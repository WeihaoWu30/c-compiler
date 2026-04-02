#pragma once
#include "tacky/top_level/identifier.hpp"
#include "tacky/abstract/instruction.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace tacky
{
  struct Function
  {
    std::unique_ptr<Identifier> identifier;
    std::vector<std::unique_ptr<Instruction>> body;
    Function(std::unique_ptr<Identifier> identifer_, std::vector<std::unique_ptr<Instruction>> body_) : identifier(std::move(identifer_)), body(std::move(body_)) {}
  };
}