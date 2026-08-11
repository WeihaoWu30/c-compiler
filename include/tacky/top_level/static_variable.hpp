#pragma once
#include "tacky/top_level/identifier.hpp"
#include "tacky/top_level/identifier.hpp"
#include <memory>
#include <utility>

namespace tacky {
  struct Static_Variable {
    std::unique_ptr<Identifier> identifier;
    int init;
    bool global;
    Static_Variable(std::unique_ptr<Identifier> identifier_, int init_, bool global_) : identifier(std::move(identifier_)), init(init_), global(global_) {}
  };
}