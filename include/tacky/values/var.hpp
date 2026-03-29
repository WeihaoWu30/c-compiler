#pragma once
#include "tacky/abstract/val.hpp"
#include "tacky/top_level/identifier.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct Var : Val
  {
    std::shared_ptr<Identifier> identifier;
    Var(std::shared_ptr<Identifier> identifier_) : identifier(std::move(identifier_)) {}
  };
}