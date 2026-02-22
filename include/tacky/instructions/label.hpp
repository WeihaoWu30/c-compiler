#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/top_level/identifier.hpp"

namespace tacky
{
  struct Label : Instruction
  {
    Identifier *identifier;
    Label(Identifier *identifer_) : identifier(identifer_) {}
    ~Label()
    {
      delete identifier;
    }
  };
}