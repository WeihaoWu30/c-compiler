#pragma once
#include "ast/top_level/identifier.hpp"

namespace ast {
  struct Declaration {
    Identifier* name;
    virtual ~Declaration() { delete name; }
  };
} // namespace ast