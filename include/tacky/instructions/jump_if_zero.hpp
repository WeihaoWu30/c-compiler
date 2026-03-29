#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"
#include "tacky/top_level/identifier.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct JumpIfZero : Instruction
  {
    std::shared_ptr<Val> condition;
    std::shared_ptr<Identifier> target;
    JumpIfZero(std::shared_ptr<Val> condition_, std::shared_ptr<Identifier> target_) : condition(std::move(condition_)), target(std::move(target_)) {}
  };
}