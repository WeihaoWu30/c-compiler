#pragma once
#include "ast/abstract/expression.hpp"
#include <ostream>

namespace ast
{
    struct Constant : Expression
    {
    public:
        int val;
        Constant(int val_) : val(val_) {}

    protected:
        void print(std::ostream &ostr) const override
        {
            ostr << "Constant(" << val << ")";
        }
    };
}