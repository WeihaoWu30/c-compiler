#pragma once
#include "aast/abstract/instruction.hpp"
#include <ostream>
#include <string>

namespace aast {
  struct Ret : Instruction {
  public:
    std::string name;
    Ret() { name = "ret"; }

  protected:
    void write(std::ostream& ostr) const override {
      ostr << "\tmovq\t%rbp, %rsp\n";
      ostr << "\tpopq\t%rbp\n";
      ostr << "\t" << name << "\n";
    }
  };
} // namespace aast