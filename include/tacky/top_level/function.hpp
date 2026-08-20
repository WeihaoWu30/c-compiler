#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"
#include "tacky/top_level/identifier.hpp"
#include <memory>
#include <utility>
#include <vector>

namespace tacky {
  struct Function {
    std::unique_ptr<Identifier> identifier;
    std::vector<std::unique_ptr<Identifier>> params;
    std::vector<std::unique_ptr<Instruction>> body;
    std::vector<std::unique_ptr<Val>> values;
    bool global;
    Function(std::unique_ptr<Identifier> identifer_, std::vector<std::unique_ptr<Identifier>> params_, std::vector<std::unique_ptr<Instruction>> body_, std::vector<std::unique_ptr<Val>> values_,
             bool global_)
        : identifier(std::move(identifer_)), params(std::move(params_)), body(std::move(body_)), values(std::move(values_)), global(global_) {}
  };
} // namespace tacky