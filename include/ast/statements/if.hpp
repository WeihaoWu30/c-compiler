#pragma once
#include "ast/abstract/statement.hpp"
#include "ast/abstract/expression.hpp"

namespace ast
{
    struct If : Statement
    {
        Expression *condition;
        Statement *then_statement;
        Statement *else_statement;
        If(Expression *condition_, Statement *then_statement_, Statement *else_statement_=nullptr) : condition(condition_), then_statement(then_statement_), else_statement(else_statement_) {}
    };
}