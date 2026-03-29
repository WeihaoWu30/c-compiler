#pragma once
#include "aast/top_level/identifier.hpp"
#include "aast/abstract/instruction.hpp"
#include <list>
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Function
  {
    std::shared_ptr<Identifier> name;
    std::list<std::unique_ptr<Instruction>> instructions;
    Function(std::shared_ptr<Identifier> name_, std::list<std::unique_ptr<Instruction>> &instructions_)
        : name(std::move(name_)), instructions(std::move(instructions_)) {}
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Function &function);
  };
}