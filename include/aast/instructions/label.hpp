#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/top_level/identifier.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Label : Instruction
  {
  public:
    std::shared_ptr<Identifier> identifier;
    Label(std::shared_ptr<Identifier> identifier_) : identifier(std::move(identifier_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "L" << *identifier << ":" << std::endl;
    }
  };
}