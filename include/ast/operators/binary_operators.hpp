#pragma once
#include <cstdint>

namespace ast
{
    enum class Binary_Operator : uint8_t
    {
        Add,
        Subtract,
        Multiply,
        Divide,
        Remainder,
        And,
        Or,
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
