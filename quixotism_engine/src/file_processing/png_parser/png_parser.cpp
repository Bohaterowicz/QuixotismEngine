#include "png_parser.hpp"

#include <array>
#include <vector>

#include "bits.hpp"
#include "dbg_print.hpp"

namespace quixotism {

static constexpr u32 PNG_HUFFMAN_MAX_BIT_COUNT = 16;

struct ExtraLength {
  u16 val;
  u16 bits;
};

static constexpr ExtraLength extra_lengths[] = {
    {3, 0},    // 257
    {4, 0},    // 258
    {5, 0},    // 259
    {6, 0},    // 260
    {7, 0},    // 261
    {8, 0},    // 262
    {9, 0},    // 263
    {10, 0},   // 264
    {11, 1},   // 265
    {13, 1},   // 266
    {15, 1},   // 267
    {17, 1},   // 268
    {19, 2},   // 269
    {23, 2},   // 270
    {27, 2},   // 271
    {31, 2},   // 272
    {35, 3},   // 273
    {43, 3},   // 274
    {51, 3},   // 275
    {59, 3},   // 276
    {67, 4},   // 277
    {83, 4},   // 278
    {99, 4},   // 279
    {115, 4},  // 280
    {131, 5},  // 281
    {163, 5},  // 282
    {195, 5},  // 283
    {227, 5},  // 284
    {258, 0},  // 285
};

static constexpr ExtraLength extra_dist[] = {
    {1, 0},       // 0
    {2, 0},       // 1
    {3, 0},       // 2
    {4, 0},       // 3
    {5, 1},       // 4
    {7, 1},       // 5
    {9, 2},       // 6
    {13, 2},      // 7
    {17, 3},      // 8
    {25, 3},      // 9
    {33, 4},      // 10
    {49, 4},      // 11
    {65, 5},      // 12
    {97, 5},      // 13
    {129, 6},     // 14
    {193, 6},     // 15
    {257, 7},     // 16
    {385, 7},     // 17
    {513, 8},     // 18
    {769, 8},     // 19
    {1025, 9},    // 20
    {1537, 9},    // 21
    {2049, 10},   // 22
    {3073, 10},   // 23
    {4097, 11},   // 24
    {6145, 11},   // 25
    {8193, 12},   // 26
    {12289, 12},  // 27
    {16385, 13},  // 28
    {24577, 13},  // 29
};

inline static constexpr u32 ReverseBits(u32 value, u32 bit_count) {
  u32 result = 0;
  for (u32 bit_idx = 0; bit_idx <= (bit_count / 2); ++bit_idx) {
    auto inv = (bit_count - (bit_idx + 1));
    result |= ((value >> bit_idx) & 0x1) << inv;
    result |= ((value >> inv) & 0x1) << bit_idx;
  }
  return result;
}

class BufferParser {
 public:
  using Buffer = std::pair<void *, size_t>;
  BufferParser() {}

  void Add(void *buffer, size_t size) {
    stream.emplace_back(buffer, size);
    if (remaining_size == 0) {
      remaining_size = stream[idx].second;
      data = stream[idx].first;
    }
  }

  void *ParseSize(size_t size) {
    void *result = nullptr;

    // Ensure that we got a buffer to parse from
    EnsureParseBuffer();

    if (remaining_size >= size) {
      result = data;
      data = ((u8 *)data) + size;
      remaining_size -= size;
    } else {
      underflow = true;
      remaining_size = 0;
    }

    Assert(!underflow);
    return result;
  }

  template <typename T>
  T *Parse() {
    T *result = reinterpret_cast<T *>(ParseSize(sizeof(T)));
    return result;
  }

  size_t GetRemainingParseBufferSize() const { return remaining_size; }

  void EnsureParseBuffer() {
    if (remaining_size == 0 && idx < stream.size()) {
      ++idx;
      remaining_size = stream[idx].second;
      data = stream[idx].first;
    }
  }

  u32 PeekBits(u32 bcount) {
    Assert(bcount <= 32);
    u32 result = 0;
    while ((bit_count < bcount) && !underflow) {
      u32 byte = *Parse<u8>();
      bit_buffer |= (byte << bit_count);
      bit_count += 8;
    }

    result = bit_buffer & ((1 << bcount) - 1);
    return result;
  }

  void DiscardBits(u32 bcount) {
    bit_count -= bcount;
    bit_buffer >>= bcount;
  }

  u32 ParseBits(u32 bcount) {
    u32 result = PeekBits(bcount);
    DiscardBits(bcount);

    return result;
  }

  void FlushByte() {
    u32 bits_to_flush = bit_count % 8;
    ParseBits(bits_to_flush);
  }

  void NextBuffer() {
    if (idx < (stream.size() - 1)) {
      ++idx;
      remaining_size = stream[idx].second;
      data = stream[idx].first;
      FlushByte();
    }
  }

  bool Finished() const {
    return (idx == (stream.size() - 1)) && remaining_size == 0;
  }

 private:
  std::vector<Buffer> stream;

  size_t idx = 0;
  void *data = nullptr;
  size_t remaining_size = 0;

  u32 bit_buffer = 0;
  u32 bit_count = 0;
  bool underflow = false;
};

class HuffmanTable {
 public:
  struct Entry {
    u16 symbol;
    u16 bits_used;
  };

  HuffmanTable(u32 max_bits)
      : max_code_length_bits{max_bits},
        entry_count{(1ULL << max_code_length_bits)} {
    Assert(max_code_length_bits <= PNG_HUFFMAN_MAX_BIT_COUNT);

    entries = std::make_unique<Entry[]>(entry_count);
  }

  void Construct(u32 count, u32 *code_lengths_in_bits) {
    u32 code_length_histogram[PNG_HUFFMAN_MAX_BIT_COUNT] = {};
    for (u32 symbol_idx = 0; symbol_idx < count; ++symbol_idx) {
      auto bit_len = code_lengths_in_bits[symbol_idx];
      Assert(bit_len < ArrayCount(code_length_histogram));
      ++code_length_histogram[bit_len];
    }

    u32 next_unused_code[PNG_HUFFMAN_MAX_BIT_COUNT];
    next_unused_code[0] = 0;
    code_length_histogram[0] = 0;
    for (u32 bit_idx = 1; bit_idx < ArrayCount(next_unused_code); ++bit_idx) {
      next_unused_code[bit_idx] =
          ((next_unused_code[bit_idx - 1] + code_length_histogram[bit_idx - 1])
           << 1);
    }

    for (u32 idx = 0; idx < count; ++idx) {
      auto length_in_bits = code_lengths_in_bits[idx];
      if (length_in_bits) {
        Assert(length_in_bits < ArrayCount(next_unused_code));
        auto code = next_unused_code[length_in_bits]++;
        auto arbitrary_bits = max_code_length_bits - length_in_bits;
        auto arbitrary_entry_count = (1 << arbitrary_bits);
        for (u32 entry_idx = 0; entry_idx < arbitrary_entry_count;
             ++entry_idx) {
          auto base_idx = ((code << arbitrary_bits) | entry_idx);
          u32 index = ReverseBits(base_idx, max_code_length_bits);

          auto &entry = entries[index];
          u32 symbol = idx;
          entry.bits_used = (u16)length_in_bits;
          entry.symbol = (u16)symbol;

          Assert(entry.bits_used == length_in_bits);
          Assert(entry.symbol == symbol);
        }
      }
    }
  }

  u32 Decode(BufferParser &parser) {
    auto idx = parser.PeekBits(max_code_length_bits);
    Assert(idx < entry_count);

    auto entry = entries[idx];
    u32 result = entry.symbol;
    Assert(entry.bits_used);
    parser.DiscardBits(entry.bits_used);
    return result;
  }

 private:
  u32 max_code_length_bits;
  size_t entry_count;
  std::unique_ptr<Entry[]> entries;
};

static inline u8 PaethPredictor(u8 a, u8 b, u8 c) {
  i32 p = (i32)a + (i32)b - (i32)c;
  u32 pa = std::abs(p - (i32)a);
  u32 pb = std::abs(p - (i32)b);
  u32 pc = std::abs(p - (i32)c);
  if (pa <= pb && pa <= pc) {
    return a;
  } else if (pb <= pc) {
    return b;
  } else {
    return c;
  }
}

static void ApplyFilterReconstruction(u8 *src, Bitmap &bitmap) {
  auto dest = bitmap.GetBitmapWritePtr();
  auto [width, height] = bitmap.GetDim();

  u8 *prev_row = nullptr;
  auto scanline_size = width * 4;
  for (u32 y = 0; y < height; ++y) {
    u8 filter = *src++;
    u8 *curr_row = dest;
    switch (filter) {
      case 0: {
        // Filter 0: Copy data from source to destination for current scanline
        std::memcpy(dest, src, scanline_size);
        dest += scanline_size;
        src += scanline_size;
      } break;
      case 1: {
        // Filter 1: Add channel value from previous pixel from the current
        // scanline
        u32 zero_pixel = 0;
        auto prev_pixel_channel = (u8 *)(&zero_pixel);
        auto curr_pixel = (u32 *)dest;
        for (u32 x = 0; x < width; ++x) {
          *dest++ = prev_pixel_channel[0] + *src++;
          *dest++ = prev_pixel_channel[1] + *src++;
          *dest++ = prev_pixel_channel[2] + *src++;
          *dest++ = prev_pixel_channel[3] + *src++;
          prev_pixel_channel = (u8 *)curr_pixel++;
        }
      } break;
      case 2: {
        // Filter 2: Add channel value from corresponding pixel from previous
        // scanline
        u32 zero_pixel = 0;
        auto top_pixel_channel = (prev_row) ? prev_row : (u8 *)(&zero_pixel);
        for (u32 x = 0; x < width; ++x) {
          *dest++ = top_pixel_channel[0] + *src++;
          *dest++ = top_pixel_channel[1] + *src++;
          *dest++ = top_pixel_channel[2] + *src++;
          *dest++ = top_pixel_channel[3] + *src++;
          top_pixel_channel =
              (prev_row) ? (top_pixel_channel + 4) : (u8 *)(&zero_pixel);
        }
      } break;
      case 3: {
        // Filter 3: Average values from channels from previous pixel form
        // current scanline and from corresponding pixel from previous scanline
        u32 zero_pixel = 0;
        auto top_pixel_channel = (prev_row) ? prev_row : (u8 *)(&zero_pixel);
        u32 prev_pixel = 0;
        auto prev_pixel_channel = (u8 *)(&prev_pixel);
        auto curr_pixel = (u32 *)dest;
        for (u32 x = 0; x < width; ++x) {
          *dest++ =
              *src++ +
              (u8)(((u32)prev_pixel_channel[0] + (u32)top_pixel_channel[0]) /
                   2U);
          *dest++ =
              *src++ +
              (u8)(((u32)prev_pixel_channel[1] + (u32)top_pixel_channel[1]) /
                   2U);
          *dest++ =
              *src++ +
              (u8)(((u32)prev_pixel_channel[2] + (u32)top_pixel_channel[2]) /
                   2U);
          *dest++ =
              *src++ +
              (u8)(((u32)prev_pixel_channel[3] + (u32)top_pixel_channel[3]) /
                   2U);
          top_pixel_channel =
              (prev_row) ? (top_pixel_channel + 4) : (u8 *)(&zero_pixel);
          prev_pixel = *curr_pixel++;
        }
      } break;
      case 4: {
        // Filter 4: Paeth Predictor
        u32 zero_pixel = 0;
        auto prev_top_pixel_channel = (u8 *)&zero_pixel;
        auto top_pixel_channel = (prev_row) ? prev_row : (u8 *)(&zero_pixel);
        u32 prev_pixel = 0;
        auto prev_pixel_channel = (u8 *)(&prev_pixel);
        auto curr_pixel = (u32 *)dest;
        for (u32 x = 0; x < width; ++x) {
          *dest++ = *src++ + PaethPredictor(prev_pixel_channel[0],
                                            top_pixel_channel[0],
                                            prev_top_pixel_channel[0]);
          *dest++ = *src++ + PaethPredictor(prev_pixel_channel[1],
                                            top_pixel_channel[1],
                                            prev_top_pixel_channel[1]);
          *dest++ = *src++ + PaethPredictor(prev_pixel_channel[1],
                                            top_pixel_channel[1],
                                            prev_top_pixel_channel[1]);
          *dest++ = *src++ + PaethPredictor(prev_pixel_channel[1],
                                            top_pixel_channel[1],
                                            prev_top_pixel_channel[1]);
          prev_top_pixel_channel = top_pixel_channel;
          top_pixel_channel =
              (prev_row) ? (top_pixel_channel + 4) : (u8 *)(&zero_pixel);
          prev_pixel = *curr_pixel++;
        }
      } break;
      default: {
        Assert(!"unknown filter type");
      } break;
    }
    prev_row = curr_row;
  }
}

std::expected<Bitmap, BitmapError> ParsePNG(void *data, size_t size) {
  BufferParser parser;
  parser.Add(data, size);
  auto *header = parser.Parse<PNGHeader>();
  bool supported = false;
  PNGIHDR *ihdr = nullptr;
  bool all_chunks = false;
  while (!all_chunks) {
    auto *chunk_header = parser.Parse<PNGChunkHeader>();
    ByteSwap32(chunk_header->length);
    auto *chunk_data = parser.ParseSize(chunk_header->length);
    switch (chunk_header->type_u32) {
      case FOURCC("IHDR"): {
        ihdr = reinterpret_cast<PNGIHDR *>(chunk_data);
        ByteSwap32(ihdr->width);
        ByteSwap32(ihdr->height);
        if (ihdr->bit_depth == 8 && ihdr->color_type == 6 &&
            ihdr->compression_method == 0 && ihdr->filter_method == 0 &&
            ihdr->interlace_method == 0) {
          supported = true;
        }
      } break;
      case FOURCC("IDAT"): {
        parser.Add(chunk_data, chunk_header->length);
      } break;
      case FOURCC("IEND"): {
        DBG_PRINT("parsed whole png");
        all_chunks = true;
      } break;
    }
    auto chunk_footer = parser.Parse<PNGChunkFooter>();
    ByteSwap32(chunk_footer->CRC);
  }

  parser.NextBuffer();

  if (supported) {
    auto *zlib_header = parser.Parse<ZLIBHeader>();
    supported = (zlib_header->cm == 8) && (zlib_header->fdict == 0);
    if (supported) {
      auto uncompressed_buffer = std::make_unique<u8[]>(
          (ihdr->width * ihdr->height * 4) + ihdr->height);
      auto uncompressed_data = uncompressed_buffer.get();
      auto dest = uncompressed_data;

      // Literal or length huffman table - stores literal or length to copy
      HuffmanTable lit_len_huffman{15};
      // Distance huffman table - stores the lookback distances if we are
      // copying lenghts from destiantion
      HuffmanTable dist_huffman{15};
      // Code length huffman table (dictionary for decoding the actual code
      // lengths in dynamic huffman case)
      HuffmanTable dict_huffman{7};

      // BFINAL bit specifies if the block we are currently consuming is the
      // last block in the comrpessed data stream
      u32 bfinal = 0;
      while (!bfinal) {
        // Get the BFINAL bit
        bfinal = parser.ParseBits(1);

        // BTYPE specifies how the data is compressed - 2 bits:
        // 00 - no compression
        // 01 - compressed with fixed Huffman codes
        // 10 - compressed with dynamic Huffman codes
        // 11 - reserved (error)
        u32 btype = parser.ParseBits(2);
        if (btype == 3) {
          Assert(0);
          // error
        } else if (btype == 0) {
          // no compression
          parser.FlushByte();
          auto len = *parser.Parse<u16>();
          auto nlen = *parser.Parse<u16>();
          Assert((u16)len == (u16)~nlen);

          // We consume the LEN bytes from stream buffer, this is uncompressed
          // pixel data from current block
          while (len) {
            // This calls make sure that the parser updates its current parsing
            // buffer, this is necessary as its possible that we will parse the
            // whole current parse buffer of the parser (if the len is very
            // big), by calling this method we ensure taht if we parse all data
            // from current parse buffer, we will move to the next one if
            // possible
            parser.EnsureParseBuffer();
            auto max_parse_size = parser.GetRemainingParseBufferSize();
            auto parse_size = len;
            if (parse_size > max_parse_size) {
              parse_size = max_parse_size;
            }
            auto src = (u8 *)parser.ParseSize(parse_size);
            Assert(src);
            std::memcpy(dest, src, parse_size);
            dest += parse_size;
            src += parse_size;
            len -= parse_size;
          }
        } else {
          // We have two modes in LZ compression:
          // 1. Literal - we copy new data to output stream (LIT)
          // 2. Length+distance - we copy old data that we already wrote, by
          // specifing how far back into the output stream to look for the data
          // (distance) and how much data to copy (length)
          //
          // We first build the huffman tables (either btype 1 or 2), then apply
          // those

          // The buffer to store the decoded length codes
          u32 code_length_table[512] = {};

          // Number of literal/length codes
          u32 hlit = 0;
          // Number of distance codes
          u32 hdist = 0;

          if (btype == 2) {
            // Dynamic huffman - We build Huffman 'tree' from the compressed
            // data stream

            hlit = parser.ParseBits(5) + 257;
            hdist = parser.ParseBits(5) + 1;

            // Number of code length codes
            u32 hclen = parser.ParseBits(4) + 4;

            // HCLEN_Swizzle specifies the order of indexes (codes) to use to
            // fill the HCLEN_Table, this order is specified by zlib spec, and
            // is used most likely because this order gives most optimal
            // compactness as in some cases you may need not to store the HCLEN
            // values that correspond to the later indexes, while the first
            // indexes are most likely to occur. Meaning the HCLEN value can be
            // less than size of this array, in that case we wont need to read
            // the values that are out of range (and those values wont be
            // written, thus saving space)
            u32 hclen_swizzle[] = {16, 17, 18, 0, 8,  7, 9,  6, 10, 5,
                                   11, 4,  12, 3, 13, 2, 14, 1, 15};
            Assert(hclen <= ArrayCount(hclen_swizzle));

            // Table for code length codes alphabet
            u32 hclen_table[ArrayCount(hclen_swizzle)] = {};

            // We fill code length table for code lengths by iterating over the
            // number of code length codes and consuming 3 bits for each code.
            // We use the swizzle table to find the proper index, since the
            // order in witch the code lengths come are important, because the
            // order and the code lengths are what is used to determine the
            // codes themself.
            for (u32 idx = 0; idx < hclen; ++idx) {
              hclen_table[hclen_swizzle[idx]] = parser.ParseBits(3);
            }

            // We construct the huffman table from the code length codes, this
            // will give us code lenghts. The 3-bit code lengths that we read
            // above are used to build the huffman alphabet which we are gonna
            // use to decode the upcoming code lengths for literals/lengths and
            // distances
            dict_huffman.Construct(ArrayCount(hclen_table), hclen_table);

            // Total count for lengths of both literal/length codes and distance
            // codes
            u32 code_length_count = hlit + hdist;
            u32 code_idx = 0;

            // We are going to keep writing values to LitLenDistTable untill we
            // wrote all HDIST+HLIT values
            while (code_idx < code_length_count) {
              u32 encoded_len = dict_huffman.Decode(parser);
              // The meaning of the decoded code lengths is as follows:
              /*
              0 - 15: Represent code lengths of 0 - 15
              16: Copy the previous code length 3 - 6 times.
                  The next 2 bits indicate repeat length
                          (0 = 3, ... , 3 = 6)
                      Example:  Codes 8, 16 (+2 bits 11),
                                  16 (+2 bits 10) will expand to
                                  12 code lengths of 8 (1 + 6 + 5)
              17: Repeat a code length of 0 for 3 - 10 times.
                  (3 bits of length)
              18: Repeat a code length of 0 for 11 - 138 times
                  (7 bits of length)
              */
              u32 repeat_count = 1;
              u32 repeat_val = 0;
              if (encoded_len <= 15) {
                repeat_val = encoded_len;
              } else if (encoded_len == 16) {
                repeat_count = parser.ParseBits(2) + 3;
                Assert(code_idx > 0);
                repeat_val = code_length_table[code_idx - 1];
              } else if (encoded_len == 17) {
                repeat_count = parser.ParseBits(3) + 3;
              } else if (encoded_len == 18) {
                repeat_count = parser.ParseBits(7) + 11;
              } else {
                Assert(!"Should never get anything else than specfied above");
              }

              while (repeat_count--) {
                code_length_table[code_idx++] = repeat_val;
              }
            }
            Assert(code_idx == code_length_count);
          } else if (btype == 1) {
            // Fixed huffman
            hlit = 288;
            hdist = 32;

            u32 bit_counts[5][2] = {
                {143, 8}, {255, 9}, {279, 7}, {287, 8}, {319, 5}};

            u32 bit_count_idx = 0;
            for (auto &bit_count_entry : bit_counts) {
              u32 bit_count = bit_count_entry[1];
              while (bit_count_idx <= bit_count_entry[0]) {
                code_length_table[bit_count_idx++] = bit_count;
              }
            }
          } else {
            // Invalid BTYPE
            Assert(0);
          }

          // Now that we got the decoded code lengths, we build the huffman
          // tables (alphabets) for the literals/lenghts and distances
          lit_len_huffman.Construct(hlit, code_length_table);
          dist_huffman.Construct(hdist, code_length_table + hlit);

          for (;;) {
            // Get the literal or length value from huffman
            u32 lit_or_len = lit_len_huffman.Decode(parser);
            if (lit_or_len <= 255) {
              // lit_or_len is a literal (<255 means literal), copy it straight
              // into result buffer
              *dest++ = (u8)(lit_or_len & 0xFF);
            } else if (lit_or_len >= 257) {
              // lit_or_len is a length,

              // Since values of 0-256 are literal values (+end of block), we
              // subtract 257 from the value (not 256 to be 0-indexed, since the
              // smallest value in this case is 257). This gives us the index
              // into the extra bits/length table.
              u32 len_idx = lit_or_len - 257;
              // We get the entry from extra_lengths table.
              // This entry specifies the repeat length and possible extra bits
              // that need to be read and then add to the length value.
              auto len_extra = extra_lengths[len_idx];
              u32 length = len_extra.val;
              if (len_extra.bits) {
                // Read the extra bits from the stream and add the value from
                // the extra bits into the length
                length += parser.ParseBits(len_extra.bits);
              }

              // After reading the length from the stream we decode the distance
              // value, which is an index into the extra_dist table
              u32 dist_idx = dist_huffman.Decode(parser);
              // Get the entry for distance value with possible extra bits
              auto dist_extra = extra_dist[dist_idx];
              u32 distance = dist_extra.val;
              if (dist_extra.bits) {
                // Read the extra bits from the stream and add the value from
                // the extra bits into the distance
                distance += parser.ParseBits(dist_extra.bits);
              }

              // Get pointer to data where we are supposed to copy data from, by
              // taking our current destination pointer and pointing it back
              // with our computed look-back distance
              u8 *src = dest - distance;
              // Assert that we dont ever go outside of our pixel buffer
              Assert(src >= uncompressed_data);
              // Copy the data one byte at a time, this way we support RLE
              while (length--) {
                *dest++ = *src++;
              }
            } else {
              break;
            }
          }
        }
      }

      Bitmap png_bitmap{ihdr->width, ihdr->height, Bitmap::BitmapFormat::RGBA8};
      ApplyFilterReconstruction(uncompressed_data, png_bitmap);

      return png_bitmap;
    }
  }
  return std::unexpected(BitmapError{});
}
}  // namespace quixotism
