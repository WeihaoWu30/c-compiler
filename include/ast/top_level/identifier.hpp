#pragma once
#include <string>
#include <ostream>

namespace ast
{
   struct Identifier
   {
      std::string text;
      Identifier(std::string text_) : text(text_) {}
   };
}
