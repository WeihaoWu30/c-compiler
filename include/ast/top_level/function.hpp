#pragma once
#include <ostream>
#include "ast/top_level/identifier.hpp"
#include "ast/abstract/statement.hpp"

namespace ast
{
    struct Function
    {
        Identifier *name;
        Statement *body;
        Function(Identifier *name_, Statement *body_) : name(name_), body(body_) {}
        ~Function()
        {
            delete name;
            delete body;
        }
        friend std::ostream &operator<<(std::ostream &ostr, const Function &function);
    };
}