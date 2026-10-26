#pragma once
#include "ast/abstract/type.hpp"
#include <memory>

namespace ast {
  struct Expression {
    std::shared_ptr<Type> type;
    virtual ~Expression() = default;
  };
} // namespace ast