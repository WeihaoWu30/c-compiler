#pragma once
#include "tacky/abstract/val.hpp"
#include "tacky/top_level/identifier.hpp"

namespace tacky
{
  struct Var : Val
  {
    Identifier *identifier;
    Var(Identifier *identifier_) : identifier(identifier_) {}
    ~Var() { delete identifier; }
  };
}