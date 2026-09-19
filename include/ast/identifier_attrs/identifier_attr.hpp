#pragma once
#include "ast/global_inits/global_inits.hpp"
#include <utility>
#include <variant>

namespace ast {
  struct Local_Attr {};

  struct Fun_Attr {
    bool defined;
    bool global;
    Fun_Attr(bool defined_, bool global_) : defined(defined_), global(global_) {}
  };

  struct Static_Attr {
    InitialValue init;
    bool global;
    Static_Attr(InitialValue init_, bool global_) : init(std::move(init_)), global(global_) {}
  };

  using Identifier_Attr = std::variant<Local_Attr, Fun_Attr, Static_Attr>;
} // namespace ast