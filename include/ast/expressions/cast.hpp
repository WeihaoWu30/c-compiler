#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/types/types.hpp"

namespace ast {
  struct Cast : Expression {
    Type* target_type;
    Expression* expression;
    Cast(Type* target_type_, Expression* expression_) : target_type(target_type_), expression(expression_) {}
  };
} // namespace ast