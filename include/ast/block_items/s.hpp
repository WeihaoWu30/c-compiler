#pragma once
#include "ast/abstract/block_item.hpp"
#include "ast/abstract/statement.hpp"
#include <ostream>

namespace ast
{
   struct S : Block_Item
   {
   public:
      Statement *statement;
      S(Statement *statement_) : statement(statement_) {};
      ~S() { delete statement; }

   protected:
      void print(std::ostream &ostr) const override {};
   };
}