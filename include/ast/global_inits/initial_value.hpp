#pragma once
#include "ast/global_inits/static_init.hpp"
#include <utility>
#include <variant>

namespace ast {
  struct Tentative {};

  struct Initial {
    StaticInit value;
    Initial(StaticInit value_) : value(std::move(value_)) {}
  };

  struct NoInitializer {};
  using InitialValue = std::variant<NoInitializer, Tentative, Initial>;
} // namespace ast