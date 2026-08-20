#pragma once
#include "ast/abstract/statement.hpp"
#include "ast/top_level/identifier.hpp"

namespace ast {
  struct Break : Statement {
    Identifier* label;
    Break(Identifier* label_) : label(label_) {}
    ~Break() { delete label; }
  };
} // namespace ast