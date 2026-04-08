#pragma once
#include <cstdint>

namespace tacky
{
  enum class Binary_Operator : uint8_t
  {
    Add,
    Subtract,
    Multiply,
    Divide,
    Remainder,
    Equal,
    NotEqual,
    LessThan,
    LessOrEqual,
    GreaterThan,
    GreaterOrEqual,
    BitAnd,
    BitOr,
    BitXor,
    BitRightShift,
    BitLeftShift,
    Invalid
  };
}