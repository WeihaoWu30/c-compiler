#pragma once
#include "ast/abstract/statement.hpp"
#include "ast/abstract/expression.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
    struct Return : Statement
    {
    public:
        std::unique_ptr<Expression> exp;
        Return(std::unique_ptr<Expression> exp_) : exp(std::move(exp_)) {}

    protected:
        void print(std::ostream &ostr) const override
        {
            ostr << "Return(" << std::endl;
            ostr << "\t\t\t" << *exp << std::endl;
            ostr << "\t\t)";
        }
    };
}