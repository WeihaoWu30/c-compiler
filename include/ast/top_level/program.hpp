#pragma once
#include "ast/top_level/function.hpp"
#include <ostream>

namespace ast
{
    struct Program
    {
        Function *func;
        Program(Function *function_definition) : func(function_definition) {}
        ~Program() { delete func; }
        friend std::ostream &operator<<(std::ostream &ostr, const Program &program);
    };
}