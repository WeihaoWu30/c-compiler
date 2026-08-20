#pragma once
#include "aast/top_level/function.hpp"
#include "aast/top_level/static_variable.hpp"
#include <ostream>
#include <utility>
#include <variant>
#include <vector>

namespace aast {
  using Top_Level = std::variant<Function, Static_Variable>;
  struct Program {
    std::vector<Top_Level> top_levels;
    Program(std::vector<Top_Level> top_levels_) : top_levels(std::move(top_levels_)) {}
    friend std::ostream& operator<<(std::ostream& ostr, const Program& program);
  };
} // namespace aast