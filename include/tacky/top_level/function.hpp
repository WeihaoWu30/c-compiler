#pragma once
#include "tacky/top_level/identifier.hpp"
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace tacky
{
  struct Function
  {
    Identifier *identifier;
    std::vector<std::unique_ptr<Instruction>> body;
    std::vector<std::unique_ptr<Val>> values;
    Function(Identifier *identifer_,
             std::vector<std::unique_ptr<Instruction>> body_,
             std::vector<std::unique_ptr<Val>> values_) : identifier(identifer_), body(std::move(body_)), values(std::move(values_)) {}
    ~Function()
    {
      delete identifier;
    }
  };
}