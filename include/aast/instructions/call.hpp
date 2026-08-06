#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/top_level/identifier.hpp"
#include <ostream>

namespace aast
{
  struct Call : Instruction
  {
    Identifier *identifier;
    Call(Identifier *identifier_) : identifier(identifier_) {}
    // add underscore for macos in front of identifier and remove the @PLT
    void write(std::ostream &ostr) const override { ostr << "call\t" << *identifier << "@PLT" << std::endl; } 
  };
}