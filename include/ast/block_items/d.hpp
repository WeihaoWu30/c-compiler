#pragma once
#include "ast/abstract/block_item.hpp"
#include "ast/top_level/declaration.hpp"
#include <ostream>

namespace ast
{
   struct D : Block_Item
   {
   public:
      Declaration *declaration;
      D(Declaration *declaration_) : declaration(declaration_) {};
      ~D() { delete declaration; }

   protected:
      void print(std::ostream &ostr) const override { ostr << std::endl; };
   };
}