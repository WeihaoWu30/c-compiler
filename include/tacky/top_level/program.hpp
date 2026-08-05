#pragma once
#include "tacky/top_level/function.hpp"
#include <vector>
#include <memory>

namespace tacky
{
  struct Program
  {
    std::vector<std::unique_ptr<Function>> function_definitions;
    Program(std::vector<std::unique_ptr<Function>> function_definitions_) : function_definitions(std::move(function_definitions_)) {}
  };
}