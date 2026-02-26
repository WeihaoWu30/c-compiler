#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/top_level/identifier.hpp"
#include <ostream>

namespace aast
{
  struct Jmp : Instruction
  {
  public:
    Identifier *identifier;
    Jmp(Identifier *identifier_) : identifier(identifier_) {}
    ~Jmp()
    {
      delete identifier;
    }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "jmp\tL" << *identifier << std::endl;
    }
  };
}