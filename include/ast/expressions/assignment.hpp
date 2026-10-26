#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/abstract/type.hpp"
#include <memory>

namespace ast {
  struct Assignment : Expression {
    Expression* lvalue;
    Expression* exp;
    Assignment(Expression* lvalue_, Expression* exp_, std::shared_ptr<Type> type_ = nullptr) : lvalue(lvalue_), exp(exp_) {
      type = type_;
    };
  };
} // namespace ast