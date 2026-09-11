#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/operators/unary_operators.hpp"
#include "ast/abstract/type.hpp"
#include <memory>

namespace ast {
  struct Unary : Expression {
    Unary_Operator unary_operator;
    Expression* exp;
    Unary(Unary_Operator unary_operator_, Expression* exp_, std::shared_ptr<Type> type_ = nullptr) : unary_operator(unary_operator_), exp(exp_) {
      type = type_;
    }
  };
} // namespace ast