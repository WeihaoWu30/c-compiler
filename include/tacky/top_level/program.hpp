#pragma once
#include "tacky/top_level/function.hpp"
#include "tacky/top_level/static_variable.hpp"
#include <utility>
#include <variant>
#include <vector>

namespace tacky {
  using Top_Level = std::variant<Function, Static_Variable>;
  struct Program {
    std::vector<Top_Level> top_levels;
    Program(std::vector<Top_Level> top_levels_) : top_levels(std::move(top_levels_)) {}
  };
} // namespace tacky