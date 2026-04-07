#pragma once
#include "ast/abstract/block_item.hpp"
#include "ast/abstract/statement.hpp"

namespace ast
{
   struct S : Block_Item
   {
      Statement *statement;
      S(Statement *statement_) : statement(statement_) {};
      ~S() { delete statement; }
   };
}