#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/constants/const.hpp"
#include <utility>

namespace ast {
  struct Constant : Expression {
    Const const_type;
    Constant(Const const_type_) : const_type(std::move(const_type_)) {}
  };
} // namespace ast