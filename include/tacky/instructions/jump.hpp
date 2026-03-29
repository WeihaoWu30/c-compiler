#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/top_level/identifier.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct Jump : Instruction
  {
    std::shared_ptr<Identifier> target;
    Jump(std::shared_ptr<Identifier> target_) : target(std::move(target_)) {}
  };
}