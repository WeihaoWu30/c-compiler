#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/type.hpp"
#include <memory>
#include <utility>

namespace ast {
  struct Cast : Expression {
    Expression* expression;
    Cast(std::shared_ptr<Type> type_, Expression* expression_) : expression(expression_) {
      type = type_;
    }
  };
} // namespace ast