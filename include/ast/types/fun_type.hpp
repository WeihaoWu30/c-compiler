#pragma once
#include "ast/abstract/type.hpp"
#include <cstddef>

namespace ast {
  struct Fun_Type : Type {
    std::size_t param_count;
    Fun_Type(std::size_t param_count_) : param_count(param_count_) {}
  };
} // namespace ast