#pragma once
#include <ostream>
#include "ast/top_level/identifier.hpp"
#include "ast/abstract/statement.hpp"
#include "ast/abstract/block_item.hpp"
#include <list>

namespace ast
{
   struct Function
   {
      Identifier *name;
      std::list<Block_Item *> body;
      Function(Identifier *name_, std::list<Block_Item *> &body_) : name(name_), body(body_) {}
      ~Function()
      {
         delete name;
         for (Block_Item *b : body)
         {
            delete b;
         }
      }
      friend std::ostream &operator<<(std::ostream &ostr, const Function &function);
   };
}