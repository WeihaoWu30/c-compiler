#pragma once
#include "ast/abstract/abstract.hpp"
#include "ast/top_level/identifier.hpp"

namespace ast {
  struct DoWhile : Statement {
    Expression* condition;
    Statement* body;
    Identifier* label;
    DoWhile(Statement* body_, Expression* condition_, Identifier* label_) : condition(condition_), body(body_), label(label_) {}
    ~DoWhile() {
      delete body;
      delete label;
    }
  };
} // namespace ast