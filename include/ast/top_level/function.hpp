#pragma once
#include <ostream>
#include "ast/top_level/identifier.hpp"
#include "ast/abstract/statement.hpp"
#include "ast/abstract/block_item.hpp"
#include "ast/abstract/expression.hpp"
#include "ast/block/block.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace ast
{
   struct Function
   {
      Identifier *name;
      Block *body;
      // std::vector<std::unique_ptr<Block_Item>> body;
      std::vector<std::unique_ptr<Expression>> expressions;
      Function(Identifier *name_, Block *body_, std::vector<std::unique_ptr<Expression>> expressions_) : name(name_), body(body_), expressions(std::move(expressions_)) {}
      ~Function()
      {
         delete name;
      }
   };
}