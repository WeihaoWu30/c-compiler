#pragma once
#include <ostream>
#include "ast/top_level/identifier.hpp"
#include "ast/abstract/statement.hpp"
#include "ast/abstract/block_item.hpp"
#include <list>
#include <memory>
#include <utility>

namespace ast
{
   struct Function
   {
      std::unique_ptr<Identifier> name;
      std::list<std::unique_ptr<Block_Item>> body;
      Function(std::unique_ptr<Identifier> name_, std::list<std::unique_ptr<Block_Item>> body_) : name(std::move(name_)), body(std::move(body_)) {}
      friend std::ostream &operator<<(std::ostream &ostr, const Function &function);
   };
}