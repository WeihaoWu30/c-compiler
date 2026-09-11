#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/operators/compound_operators.hpp"
#include "ast/abstract/type.hpp"
#include <memory>

namespace ast {
  struct Compound : Expression {
    Compound_Operator compound_operator;
    Expression *left, *right;
    Compound(Compound_Operator compound_operator_, Expression* left_, Expression* right_, std::shared_ptr<Type> type_ = nullptr) : compound_operator(compound_operator_), left(left_), right(right_) {
      type = type_;
    }
  };
} // namespace ast
