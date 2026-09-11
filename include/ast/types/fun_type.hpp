#pragma once
#include "ast/abstract/type.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace ast {
  struct Fun_Type : Type {
    std::vector<std::shared_ptr<Type>> param_types;
    std::shared_ptr<Type> return_type;
    Fun_Type(std::vector<std::shared_ptr<Type>> param_types_, std::shared_ptr<Type> return_type_) : param_types(std::move(param_types_)), return_type(return_type_) {}
  };
} // namespace ast