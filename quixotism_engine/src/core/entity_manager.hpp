#pragma once
#include "entity.hpp"
#include "quixotism_c.hpp"
#include <array>
#include <list>
#include <memory>

struct entity_block
{
    static const int32 BlockSize = 256;
    size Count = 0;
    std::list<size> FreeIndicies{};
    std::array<std::unique_ptr<entity>, BlockSize> Data{};
    std::unique_ptr<entity_block> Next = nullptr;
};

class entity_manager
{
  public:
    entity_manager()
    {
        RootBlock = std::make_unique<entity_block>();
    }

    size Insert(std::unique_ptr<entity> Value)
    {
        uint32 BlockCount = 0;
        size Index = 0;
        entity_block *PreviousBlock = nullptr;
        auto *CurrentBlock = RootBlock.get();
        while (CurrentBlock)
        {
            if (CurrentBlock->Count < entity_block::BlockSize)
            {
                if (!CurrentBlock->FreeIndicies.empty())
                {
                    Index = CurrentBlock->FreeIndicies.back();
                    CurrentBlock->FreeIndicies.pop_back();
                }
                else
                {
                    Index = CurrentBlock->Count;
                    ++CurrentBlock->Count;
                }
                CurrentBlock->Data[Index] = std::move(Value);
                return BlockCount * entity_block::BlockSize + Index;
            }
            PreviousBlock = CurrentBlock;
            CurrentBlock = CurrentBlock->Next.get();
        }
        if (PreviousBlock != nullptr)
        {
            PreviousBlock->Next = std::make_unique<entity_block>();
            CurrentBlock = PreviousBlock->Next.get();
            CurrentBlock->Data[0] = std::move(Value);
            return BlockCount * entity_block::BlockSize;
        }
        return 0;
    }

    void Delete(size Index)
    {
        Assert(Index > 0);
        entity_block *PreviousBlock = nullptr;
        auto *CurrentBlock = RootBlock.get();
        while (CurrentBlock && Index > entity_block::BlockSize)
        {
            PreviousBlock = CurrentBlock;
            CurrentBlock = CurrentBlock->Next.get();
            Index -= entity_block::BlockSize;
        }
        if (CurrentBlock && CurrentBlock->Data[Index] != nullptr)
        {
            --CurrentBlock->Count;
            if (Index != CurrentBlock->Count - 1)
            {
                CurrentBlock->FreeIndicies.push_back(Index);
            }
            CurrentBlock->Data[Index].reset();
            if (CurrentBlock->Count == 0 && CurrentBlock->Next == nullptr && PreviousBlock != nullptr)
            {
                PreviousBlock->Next.release();
            }
        }
    }

    entity *operator[](size Index)
    {
        Assert(Index > 0);
        auto *CurrentBlock = RootBlock.get();
        while (CurrentBlock && Index > entity_block::BlockSize)
        {
            CurrentBlock = CurrentBlock->Next.get();
            Index -= entity_block::BlockSize;
        }
        if (CurrentBlock)
        {
            return CurrentBlock->Data[Index].get();
        }
        Assert(false);
        return RootBlock->Data[0].get();
    }

    _NODISCARD std::unique_ptr<entity_block> &GetRootBlock()
    {
        return RootBlock;
    }

  private:
    std::unique_ptr<entity_block> RootBlock = nullptr;
};