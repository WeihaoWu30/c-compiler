#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/binary_operator.hpp"

namespace ast
{
    struct Binary : Expression
    {
        Binary_Operator *binary_operator;
        Expression *left, *right;
        Binary(Binary_Operator *binary_operator_, Expression *left_, Expression *right_) : binary_operator(binary_operator_), left(left_), right(right_) {}
        ~Binary()
        {
            delete binary_operator;
        }
    };
}
