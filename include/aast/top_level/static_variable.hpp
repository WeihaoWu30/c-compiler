#pragma once
#include "aast/top_level/identifier.hpp"
#include "ast/global_inits/global_inits.hpp"
#include <memory>
#include <utility>

namespace aast {
  struct Static_Variable {
    std::unique_ptr<Identifier> name;
    int alignment;
    ast::StaticInit init;
    bool global;
    Static_Variable(std::unique_ptr<Identifier> name_, int alignment_, ast::StaticInit init_, bool global_) : name(std::move(name_)), alignment(alignment_), init(std::move(init_)), global(global_) {}
    friend std::ostream& operator<<(std::ostream& ostr, const Static_Variable& static_variable);
  };
} // namespace aast