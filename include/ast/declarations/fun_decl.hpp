#pragma once
#include "ast/abstract/declaration.hpp"
#include "ast/top_level/identifier.hpp"
#include "ast/block/block.hpp"
#include "ast/abstract/expression.hpp"
#include "ast/storage_class/storage_classes.hpp"
#include <vector>
#include <memory>

namespace ast
{
    struct Fun_Decl : Declaration
    {
        std::vector<std::unique_ptr<Identifier>> params;
        std::vector<std::unique_ptr<ast::Expression>> expressions;
        Block *body;
        Storage_Class storage_class;
        Fun_Decl(
            Identifier *name_, std::vector<std::unique_ptr<Identifier>> params_,
            std::vector<std::unique_ptr<ast::Expression>> expressions_ = {}, 
            Block *body_ = nullptr, 
            Storage_Class storage_class_ = Storage_Class::NONE
        ) : params(std::move(params_)), expressions(std::move(expressions_)), body(body_), storage_class(storage_class_) {
            name = name_; // name owned by declaration
        }
        ~Fun_Decl() {
            delete body;
        }
    };
}