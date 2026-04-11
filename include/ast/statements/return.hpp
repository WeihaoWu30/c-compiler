#pragma once
#include "ast/abstract/statement.hpp"
#include "ast/abstract/expression.hpp"

namespace ast
{
    struct Return : Statement
    {
        Expression *exp;
        Return(Expression *exp_) : exp(exp_) {}
    };
}