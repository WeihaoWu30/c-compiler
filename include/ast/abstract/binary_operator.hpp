#pragma once
#include <ostream>
#include <string>

namespace ast
{
   struct Binary_Operator
   {
   public:
      virtual ~Binary_Operator() = default;

   protected:
      std::string symbol;
      friend std::ostream &operator<<(std::ostream &ostr, const Binary_Operator &binary_operator)
      {
         ostr << binary_operator.symbol;
         return ostr;
      }
   };
}