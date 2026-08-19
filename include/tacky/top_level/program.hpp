#pragma once
#include "tacky/top_level/function.hpp"
#include "tacky/top_level/static_variable.hpp"
#include <vector>
#include <utility>
#include <variant>

namespace tacky
{
  using Top_Level = std::variant<Function, Static_Variable>;
  struct Program
  {
    std::vector<Top_Level> top_levels;
    Program(std::vector<Top_Level> top_levels_) : top_levels(std::move(top_levels_)) {}
  };
}