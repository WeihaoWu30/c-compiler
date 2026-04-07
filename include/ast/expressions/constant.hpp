#pragma once
#include "ast/abstract/expression.hpp"

namespace ast
{
    struct Constant : Expression
    {
        int val;
        Constant(int val_) : val(val_) {}
    };
}