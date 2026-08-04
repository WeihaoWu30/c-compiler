#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/top_level/identifier.hpp"
#include <vector>
#include <memory>

namespace ast
{
    struct Function_Call : Expression
    {
        Identifier *identifier;
        std::vector<Expression *> args;
        Function_Call(Identifier *identifier_, std::vector<Expression *> &args_) : identifier(identifier_), args(args_) {}
        ~Function_Call() {
            delete identifier;
        }
    };
}