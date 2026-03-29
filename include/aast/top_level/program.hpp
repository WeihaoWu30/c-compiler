#pragma once
#include "aast/top_level/function.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Program
  {
    std::unique_ptr<Function> function_definition;
    Program(std::unique_ptr<Function> function_definition_)
        : function_definition(std::move(function_definition_)) {}
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Program &program);
  };
}