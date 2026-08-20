#pragma once
#include "ast/abstract/statement.hpp"
#include "ast/top_level/identifier.hpp"

namespace ast {
  struct Continue : Statement {
    Identifier* label;
    Continue(Identifier* label_) : label(label_) {}
    ~Continue() { delete label; }
  };
} // namespace ast