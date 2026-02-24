#pragma once
#include "ast/abstract/statement.hpp"
#include "ast/abstract/expression.hpp"
#include <ostream>

namespace ast
{
    struct Return : Statement
    {
    public:
        Expression *exp;
        Return(Expression *exp_) : exp(exp_) {}
        ~Return() { delete exp; }

    protected:
        void print(std::ostream &ostr) const override
        {
            ostr << "Return(" << std::endl;
            ostr << "\t\t\t" << *exp << std::endl;
            ostr << "\t\t)";
        }
    };
}