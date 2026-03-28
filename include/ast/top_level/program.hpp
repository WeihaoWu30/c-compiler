#pragma once
#include "ast/top_level/function.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
    struct Program
    {
        std::unique_ptr<Function> func;
        Program(std::unique_ptr<Function> function_definition) : func(std::move(function_definition)) {}
        friend std::ostream &operator<<(std::ostream &ostr, const Program &program);
    };
}