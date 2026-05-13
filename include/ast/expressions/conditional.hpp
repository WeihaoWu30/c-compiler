#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/operators/binary_operators.hpp"

namespace ast
{
    struct Conditional : Expression
    {
        Expression * condition, *left, *right;
        Conditional(Expression *condition_, Expression *left_, Expression *right_) : condition(condition_), left(left_), right(right_) {}
    };
}
