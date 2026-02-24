#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"
#include "tacky/top_level/identifier.hpp"

namespace tacky
{
  struct JumpIfZero : Instruction
  {
    Val *condition;
    Identifier *target;
    JumpIfZero(Val *condition_, Identifier *target_) : condition(condition_), target(target_) {}
    ~JumpIfZero()
    {
      delete condition;
      delete target;
    }
  };
}