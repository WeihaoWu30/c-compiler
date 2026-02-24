#pragma once
#include "tacky/top_level/identifier.hpp"
#include "tacky/abstract/instruction.hpp"
#include <vector>

namespace tacky
{
  struct Function
  {
    Identifier *identifier;
    std::vector<Instruction *> body;
    Function(Identifier *identifer_, std::vector<Instruction *> &body_) : identifier(identifer_), body(body_) {}
    ~Function()
    {
      delete identifier;
      for (Instruction *&instr : body)
        delete instr;
    }
  };
}