#pragma once
#include "ast/abstract/declaration.hpp"
#include "ast/abstract/expression.hpp"
#include "ast/block/block.hpp"
#include "ast/storage_class/storage_classes.hpp"
#include "ast/top_level/identifier.hpp"
#include "ast/types/types.hpp"
#include <memory>
#include <vector>

namespace ast {
  struct Fun_Decl : Declaration {
    std::vector<std::unique_ptr<Identifier>> params;
    std::vector<std::unique_ptr<ast::Expression>> expressions;
    Block* body;
    std::unique_ptr<Type> fun_type;
    Storage_Class storage_class;
    Fun_Decl(Identifier* name_, std::unique_ptr<Type> fun_type_, std::vector<std::unique_ptr<Identifier>> params_, std::vector<std::unique_ptr<ast::Expression>> expressions_ = {},
             Block* body_ = nullptr,
             Storage_Class storage_class_ = Storage_Class::NONE)
        : params(std::move(params_)), expressions(std::move(expressions_)), body(body_), fun_type(std::move(fun_type_)), storage_class(storage_class_) {
      name = name_; // name owned by declaration
    }
    ~Fun_Decl() { delete body; }
  };
} // namespace ast