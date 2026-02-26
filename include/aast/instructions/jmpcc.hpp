#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/top_level/identifier.hpp"
#include "aast/abstract/cond_code.hpp"
#include <ostream>

namespace aast
{
  struct JmpCC : Instruction
  {
  public:
    Cond_Code *cond_code;
    Identifier *identifier;
    JmpCC(Cond_Code *cond_code_, Identifier *identifier_) : cond_code(cond_code_), identifier(identifier_) {}
    ~JmpCC()
    {
      delete cond_code;
      delete identifier;
    }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "j" << *cond_code << "\tL" << *identifier << std::endl;
    }
  };
}