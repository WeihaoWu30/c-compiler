#pragma once
#include <ostream>
#include "ast/top_level/identifier.hpp"
#include "ast/abstract/statement.hpp"
#include "ast/abstract/block_item.hpp"
#include "ast/abstract/expression.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace ast
{
   struct Function
   {
      Identifier *name;
      std::vector<std::unique_ptr<Block_Item>> body;
      std::vector<std::unique_ptr<Expression>> exprs;
      Function(Identifier *name_, std::vector<std::unique_ptr<Block_Item>> body_, std::vector<std::unique_ptr<Expression>> exprs_) : name(name_), body(std::move(body_)), exprs(std::move(exprs_)) {}
      ~Function()
      {
         delete name;
      }
   };
}