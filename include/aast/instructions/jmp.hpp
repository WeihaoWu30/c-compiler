#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/top_level/identifier.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Jmp : Instruction
  {
  public:
    std::shared_ptr<Identifier> identifier;
    Jmp(std::shared_ptr<Identifier> identifier_) : identifier(std::move(identifier_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "jmp\tL" << *identifier << std::endl;
    }
  };
}