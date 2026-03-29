#pragma once
#include "aast/abstract/operand.hpp"
#include "aast/top_level/identifier.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Pseudo : Operand
  {
  public:
    std::shared_ptr<Identifier> identifier;
    Pseudo(std::shared_ptr<Identifier> identifier_) : identifier(std::move(identifier_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << ""; // Preventing Errors when compiling since this isn't being read as an assembly line
    };
  };
}