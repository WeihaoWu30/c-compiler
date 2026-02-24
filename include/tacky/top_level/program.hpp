#pragma once
#include "tacky/top_level/function.hpp"

namespace tacky
{
  struct Program
  {
    Function *func;
    Program(Function *function_definition) : func(function_definition) {}
    ~Program() { delete func; }
  };
}