#pragma once
#include "aast/top_level/function.hpp"
#include <ostream>

namespace aast
{
  struct Program
  {
    Function *function_definition;
    Program(Function *function_definition_)
        : function_definition(function_definition_) {}
    ~Program() { delete function_definition; }
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Program &program);
  };
}