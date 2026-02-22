#pragma once
#include "aast/top_level/identifier.hpp"
#include "aast/abstract/instruction.hpp"
#include <list>
#include <ostream>

namespace aast
{
  struct Function
  {
    Identifier *name;
    std::list<Instruction *> instructions;
    Function(Identifier *name_, std::list<Instruction *> &instructions_)
        : name(name_), instructions(instructions_) {}
    ~Function()
    {
      delete name;
      for (Instruction *instr : instructions)
      {
        delete instr;
      }
    }
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Function &function);
  };
}