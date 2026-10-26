#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/constants/const.hpp"
#include "ast/abstract/type.hpp"
#include <memory> 

namespace ast {
  struct Constant : Expression {
    Const const_type;
    Constant(Const const_type_, std::shared_ptr<Type> type_ = nullptr) : const_type(std::move(const_type_)) {
      type = type_;
    }
  };
} // namespace ast