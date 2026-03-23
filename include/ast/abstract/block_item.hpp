#pragma once
#include <ostream>

namespace ast
{
   struct Block_Item
   {
   public:
      virtual ~Block_Item() = default;

   protected:
      virtual void print(std::ostream &ostr) const = 0;
   };
}