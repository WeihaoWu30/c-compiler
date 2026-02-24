#pragma once
#include "aast/abstract/instruction.hpp"
#include <string>
#include <ostream>

namespace aast
{
  struct Ret : Instruction
  {
  public:
    std::string name;
    Ret() { name = "ret"; }

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "\tmovq\t%rbp, %rsp" << std::endl;
      ostr << "\tpopq\t%rbp" << std::endl;
      ostr << "\t" << name << std::endl;
    }
  };
}