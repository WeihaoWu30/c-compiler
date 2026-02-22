#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/top_level/identifier.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky
{
  struct JumpIfNotZero : Instruction
  {
    Val *condition;
    Identifier *target;
    JumpIfNotZero(Val *condition_, Identifier *target_) : condition(condition_), target(target_) {}
    ~JumpIfNotZero()
    {
      delete condition;
      delete target;
    }
  };
}