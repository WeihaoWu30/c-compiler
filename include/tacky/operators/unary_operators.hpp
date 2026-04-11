#pragma once
#include <cstdint>

namespace tacky
{
  enum class Unary_Operator : uint8_t
  {
    Complement,
    Negate,
    Not,
    Invalid
  };
}