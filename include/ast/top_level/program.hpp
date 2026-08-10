#pragma once
#include "ast/abstract/declaration.hpp"
#include "ast/abstract/expression.hpp"
#include <vector>
#include <memory>

namespace ast
{
    struct Program
    {
        std::vector<std::unique_ptr<Declaration>> declarations;
        std::vector<std::unique_ptr<Expression>> global_expressions;
        Program(
            std::vector<std::unique_ptr<Declaration>> declarations_, 
            std::vector<std::unique_ptr<Expression>> global_expressions_
        ) : declarations(std::move(declarations_)), global_expressions(std::move(global_expressions_)) {}
    };
}