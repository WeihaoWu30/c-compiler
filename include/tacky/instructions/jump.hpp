#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/top_level/identifier.hpp"

namespace tacky
{
  struct Jump : Instruction
  {
    Identifier *target;
    Jump(Identifier *target_) : target(target_) {}
    ~Jump()
    {
      delete target;
    }
  };
}