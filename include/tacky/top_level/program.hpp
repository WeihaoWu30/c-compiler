#pragma once
#include "tacky/top_level/function.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct Program
  {
    std::unique_ptr<Function> func;
    Program(std::unique_ptr<Function> function_definition) : func(std::move(function_definition)) {}
  };
}