#pragma once
#include "ast/abstract/binary_operator.hpp"

namespace ast
{
    struct Add : Binary_Operator
    {
        Add() { symbol = "+"; }
    };

    struct Subtract : Binary_Operator
    {
        Subtract() { symbol = "-"; }
    };

    struct Multiply : Binary_Operator
    {
        Multiply() { symbol = "*"; }
    };

    struct Divide : Binary_Operator
    {
        Divide() { symbol = "/"; }
    };

    struct Remainder : Binary_Operator
    {
        Remainder() { symbol = "%"; }
    };

    struct And : Binary_Operator
    {
        And() { symbol = "&&"; }
    };

    struct Or : Binary_Operator
    {
        Or() { symbol = "||"; }
    };

    struct Equal : Binary_Operator
    {
        Equal() { symbol = "=="; }
    };

    struct NotEqual : Binary_Operator
    {
        NotEqual() { symbol = "!="; }
    };

    struct LessThan : Binary_Operator
    {
        LessThan() { symbol = "<"; }
    };

    struct LessOrEqual : Binary_Operator
    {
        LessOrEqual() { symbol = "<="; }
    };

    struct GreaterThan : Binary_Operator
    {
        GreaterThan() { symbol = ">"; }
    };

    struct GreaterOrEqual : Binary_Operator
    {
        GreaterOrEqual() { symbol = ">="; }
    };
}
