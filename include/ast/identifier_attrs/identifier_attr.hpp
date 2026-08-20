#pragma once
#include "ast/initial_values/initial_value.hpp"
#include <variant>

namespace ast {
  struct Local_Attr {};

  struct Fun_Attr {
    bool defined;
    bool global;
    Fun_Attr(bool defined_, bool global_) : defined(defined_), global(global_) {}
  };

  struct Static_Attr {
    Initial_Value init;
    bool global;
    Static_Attr(Initial_Value init_, bool global_) : init(init_), global(global_) {}
  };

  using Identifier_Attr = std::variant<Local_Attr, Fun_Attr, Static_Attr>;
} // namespace ast