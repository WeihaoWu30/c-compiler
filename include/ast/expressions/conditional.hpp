#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/type.hpp"
#include <memory>

namespace ast {
  struct Conditional : Expression {
    Expression *condition, *left, *right;
    Conditional(Expression* condition_, Expression* left_, Expression* right_, std::shared_ptr<Type> type_ = nullptr) : condition(condition_), left(left_), right(right_) {
      type = type_;
    }
  };
} // namespace ast
