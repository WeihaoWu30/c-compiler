#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/top_level/identifier.hpp"
#include <ostream>

namespace aast
{
  struct Label : Instruction
  {
  public:
    Identifier *identifier;
    Label(Identifier *identifier_) : identifier(identifier_) {}
    ~Label()
    {
      delete identifier;
    }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "L" << *identifier << ":" << std::endl;
    }
  };
}