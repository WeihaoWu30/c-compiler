#pragma once
#include "ast/abstract/block_item.hpp"
#include "ast/abstract/statement.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
   struct S : Block_Item
   {
   public:
      std::unique_ptr<Statement> statement;
      S(std::unique_ptr<Statement> statement_) : statement(std::move(statement_)) {};

   protected:
      void print(std::ostream &ostr) const override
      {
         ostr << "S Block(" << *statement << ")" << std::endl;
      };
   };
}