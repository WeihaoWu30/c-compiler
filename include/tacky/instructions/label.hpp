#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/top_level/identifier.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct Label : Instruction
  {
    std::shared_ptr<Identifier> identifier;
    Label(std::shared_ptr<Identifier> identifer_) : identifier(std::move(identifer_)) {}
  };
}