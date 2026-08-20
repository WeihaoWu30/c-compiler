#pragma once
#include "aast/top_level/identifier.hpp"
#include <memory>
#include <utility>

namespace aast {
  struct Static_Variable {
    std::unique_ptr<Identifier> name;
    int init;
    bool global;
    Static_Variable(std::unique_ptr<Identifier> name_, int init_, bool global_) : name(std::move(name_)), init(init_), global(global_) {}
    friend std::ostream& operator<<(std::ostream& ostr, const Static_Variable& static_variable);
  };
} // namespace aast