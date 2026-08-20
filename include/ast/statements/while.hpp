#pragma once
#include "ast/abstract/abstract.hpp"
#include "ast/top_level/identifier.hpp"

namespace ast {
  struct While : Statement {
    Expression* condition;
    Statement* body;
    Identifier* label;
    While(Expression* condition_, Statement* body_, Identifier* label_) : condition(condition_), body(body_), label(label_) {}
    ~While() {
      delete body;
      delete label;
    }
  };
} // namespace ast