#pragma once
#include <vector>
#include <memory>
#include "ast/block_items/block_items.hpp"

namespace ast{
   struct Block{
      std::vector<std::unique_ptr<Block_Item>> block_items;
      Block(std::vector<std::unique_ptr<Block_Item>> block_items_) : block_items(std::move(block_items_)) {}
   };
}
