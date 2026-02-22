#pragma once
#include "aast/abstract/instruction.hpp"
#include <ostream>

namespace aast
{
  struct AllocateStack : Instruction
  {
  public:
    int bytes;
    AllocateStack(int bytes_) : bytes(bytes_) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "subq\t$" << bytes << ", %rsp" << std::endl;
    }
  };
}