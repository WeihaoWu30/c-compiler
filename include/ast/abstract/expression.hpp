#pragma once
#include <ostream>
#include <string>

namespace ast
{
   struct Expression
   {
   public:
      virtual ~Expression() = default;

   protected:
      virtual void print(std::ostream &ostr) const = 0;
      friend std::ostream &operator<<(std::ostream &ostr, const Expression &exp)
      {
         exp.print(ostr);
         return ostr;
      }
   };
}