#pragma once
#include "aast/top_level/function.hpp"
#include <vector>
#include <ostream>
#include <memory>

namespace aast
{
  struct Program
  {
    std::vector<std::unique_ptr<Function>> function_definitions;
    Program(std::vector<std::unique_ptr<Function>> function_definitions_)
        : function_definitions(std::move(function_definitions_)) {}
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Program &program);
  };
}