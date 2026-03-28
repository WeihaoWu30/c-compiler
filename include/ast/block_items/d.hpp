#pragma once
#include "ast/abstract/block_item.hpp"
#include "ast/top_level/declaration.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
   struct D : Block_Item
   {
   public:
      std::unique_ptr<Declaration> declaration;
      D(std::unique_ptr<Declaration> declaration_) : declaration(std::move(declaration_)) {};

   protected:
      void print(std::ostream &ostr) const override
      {
         ostr << "D Block(" << *declaration << ")" << std::endl;
      };
   };
}