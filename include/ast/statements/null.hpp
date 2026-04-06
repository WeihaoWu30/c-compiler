#pragma once
#include "ast/abstract/statement.hpp"

namespace ast
{
   struct Null : Statement
   {
   public:
      Null() {};

   protected:
      void print(std::ostream &ostr) const override { ostr << std::endl; };
   };
}