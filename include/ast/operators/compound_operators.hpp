#pragma once
#include <cstdint>

namespace ast
{
  enum class Compound_Operator : uint8_t
  {
    AdditionAssignment,
    SubtractionAssignment,
    MultiplicationAssignment,
    DivisionAssignment,
    ModulusAssignment,
    BitwiseAndAssignment,
    BitwiseOrAssignment,
    BitwiseXorAssignment,
    LeftShiftAssignment,
    RightShiftAssignment,
    Invalid
  };
}
