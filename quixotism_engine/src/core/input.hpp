#pragma once

#include <array>
#include <cctype>

#include "quixotism_c.hpp"

namespace quixotism {
constexpr size_t MAX_KEY_STATE_SIZE = 0xFF;

// KeyCodes correspond 1-to-1 to Windows Virtual-Key codes.
enum KeyCode : u32 {
  KC_INVALID = 0x00,
  KC_LBUTTON = 0x01,
  KC_RBUTTON = 0x02,
  KC_CANCEL = 0x03,
  KC_MBUTTON = 0x04,
  KC_XBUTTON1 = 0x05,
  KC_XBUTTON2 = 0x06,
  KC_BACK = 0x08,
  KC_TAB = 0x09,
  KC_CLEAR = 0x0C,
  KC_RETURN = 0x0D,
  KC_SHIFT = 0x10,
  KC_CONTROL = 0x11,
  KC_MENU = 0x12,
  KC_PAUSE = 0x13,
  KC_CAPITAL = 0x14,

  /*
    KC_KANA =           0x15
    KC_HANGEUL =        0x15
    KC_HANGUL =         0x15
    KC_IME_ON =         0x16
    KC_JUNJA =          0x17
    KC_FINAL =          0x18
    KC_HANJA =          0x19
    KC_KANJI =          0x19
    KC_IME_OFF =        0x1A
  */

  KC_ESCAPE = 0x1B,

  /*
    KC_CONVERT =        0x1C
    KC_NONCONVERT =     0x1D
    KC_ACCEPT =        0x1E
    KC_MODECHANGE =     0x1
  */

  KC_SPACE = 0x20,
  KC_PRIOR = 0x21,
  KC_NEXT = 0x22,
  KC_END = 0x23,
  KC_HOME = 0x24,
  KC_LEFT = 0x25,
  KC_UP = 0x26,
  KC_RIGHT = 0x27,
  KC_DOWN = 0x28,
  KC_SELECT = 0x29,
  KC_PRINT = 0x2A,
  KC_EXECUTE = 0x2B,
  KC_SNAPSHOT = 0x2C,
  KC_INSERT = 0x2D,
  KC_DELETE = 0x2E,
  KC_HELP = 0x2F,
  // 0x30 - 0x39 -> Number keys (0-9) (ASCII)
  // 0x41 - 0x5A -> Character keys (A-Z) (ASCII)
  KC_PLUS = 0xBB,
  KC_MINUS = 0xBD,
};

constexpr bool ToCharacter(u32 key_code, bool shift, char &c) {
  if (key_code >= 0x41 && key_code <= 0x5A) {
    c = shift ? (char)key_code : (char)tolower(key_code);
    return true;
  }
  if (key_code >= 0x30 && key_code <= 0x39) {
    if (shift) {
      switch (key_code) {
        case 0x30:
          c = ')';
          break;
        case 0x31:
          c = '!';
          break;
        case 0x32:
          c = '@';
          break;
        case 0x33:
          c = '#';
          break;
        case 0x34:
          c = '$';
          break;
        case 0x35:
          c = '%';
          break;
        case 0x36:
          c = '^';
          break;
        case 0x37:
          c = '&';
          break;
        case 0x38:
          c = '*';
          break;
        case 0x39:
          c = '(';
          break;
      }
    } else {
      c = (char)key_code;
    }
    return true;
  }

  bool result = true;
  switch (key_code) {
    case KC_PLUS: {
      c = shift ? '+' : '=';
    } break;
    case KC_MINUS: {
      c = shift ? '_' : '-';
    } break;
    default: {
      result = false;
    } break;
  }

  return result;
}

struct KeyState {
  KeyState() = default;
  bool is_down = false;     // false=key_up, true=key_down
  bool transition = false;  // false=key_repeat, true=key_changed_state
  u32 key_code;
};

constexpr auto key_info_array_init = [] {
  std::array<KeyState, MAX_KEY_STATE_SIZE> arr;
  for (auto idx = 0; idx < MAX_KEY_STATE_SIZE; ++idx) {
    arr[idx].key_code = idx;
  }
  return arr;
}();

struct InputState {
  InputState() = default;

  std::array<KeyState, MAX_KEY_STATE_SIZE> key_state_info = key_info_array_init;
  KeyCode last_key_code = KC_INVALID;
  r32 mouse_x_delta = 0.f;
  r32 mouse_y_delta = 0.f;
};

}  // namespace quixotism
