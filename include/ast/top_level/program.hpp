#pragma once
#include "ast/declarations/fun_decl.hpp"
#include <vector>
#include <memory>

namespace ast
{
    struct Program
    {
        std::vector<std::unique_ptr<Fun_Decl>> functions_declarations;
        Program(std::vector<std::unique_ptr<Fun_Decl>> functions_declarations_) : functions_declarations(std::move(functions_declarations_)) {}
    };
}