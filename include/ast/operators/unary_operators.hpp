#pragma once
#include <cstdint>

namespace ast
{
   enum class Unary_Operator : uint8_t
   {
      Complement,
      Negate,
      Not,
      Invalid
   };
}