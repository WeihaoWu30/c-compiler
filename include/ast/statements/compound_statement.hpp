#pragma once
#include "ast/abstract/statement.hpp"
#include "ast/block/block.hpp"

namespace ast{
   struct Compound_Statement : Statement{
      Block* block;
      Compound_Statement(Block *block_) : block(block_){}
   };
}