#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/statement.hpp"

namespace ast {
  struct Return : Statement {
    Expression* exp;
    Return(Expression* exp_) : exp(exp_) {}
  };
} // namespace ast