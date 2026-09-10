#pragma once
#include "ast/abstract/declaration.hpp"
#include "ast/abstract/expression.hpp"
#include "ast/storage_class/storage_classes.hpp"
#include "ast/top_level/identifier.hpp"
#include "ast/types/types.hpp"
#include <memory>
#include <utility>

namespace ast {
  struct Var_Decl : Declaration {
    Expression* init;
    std::unique_ptr<Type> type;
    Storage_Class storage_class;
    Var_Decl(Identifier* name_, std::unique_ptr<Type> type_, Expression* init_ = nullptr, Storage_Class storage_class_ = Storage_Class::NONE) : init(init_), type(std::move(type_)), storage_class(storage_class_) {
      name = name_; // name owned by declaration
    };
  };
} // namespace ast