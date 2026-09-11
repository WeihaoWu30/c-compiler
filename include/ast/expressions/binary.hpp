#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/operators/binary_operators.hpp"
#include "ast/abstract/type.hpp"
#include <memory>

namespace ast {
  struct Binary : Expression {
    Binary_Operator binary_operator;
    Expression *left, *right;
    Binary(Binary_Operator binary_operator_, Expression* left_, Expression* right_, std::shared_ptr<Type> type_ = nullptr) : binary_operator(binary_operator_), left(left_), right(right_) {
      type = type_;
    }
  };
} // namespace ast
