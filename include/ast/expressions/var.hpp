#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/top_level/identifier.hpp"
#include "ast/abstract/type.hpp"
#include <memory>

namespace ast {
  struct Var : Expression {
    Identifier* identifier;
    Var(Identifier* identifier_, std::shared_ptr<Type> type_ = nullptr) : identifier(identifier_) {
      type = type_;
    }
    ~Var() { delete identifier; }
  };
} // namespace ast