#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/binary_operator.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
    struct Binary : Expression
    {
    public:
        std::unique_ptr<Binary_Operator> binary_operator;
        std::unique_ptr<Expression> left, right;
        Binary(std::unique_ptr<Binary_Operator> binary_operator_, std::unique_ptr<Expression> left_, std::unique_ptr<Expression> right_) : binary_operator(std::move(binary_operator_)), left(std::move(left_)), right(std::move(right_)) {}

    protected:
        void print(std::ostream &ostr) const override
        {
            ostr << "Binary Operator(" << *binary_operator << ", " << *left << ", " << *right << ")";
        }
    };
}
