#pragma once
#include <string>
#include <ostream>

namespace ast
{
   struct Identifier
   {
      std::string name;
      Identifier(std::string name_) : name(name_) {}
   };
}
