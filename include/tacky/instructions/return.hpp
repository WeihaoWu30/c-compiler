#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct Return : Instruction
  {
    std::unique_ptr<Val> val;
    Return(std::unique_ptr<Val> val_) : val(std::move(val_)) {}
  };
}