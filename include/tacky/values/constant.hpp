#pragma once
#include "tacky/abstract/val.hpp"
#include "ast/constants/const.hpp"
#include <utility>

namespace tacky {
  struct Constant : Val {
    ast::Const val;
    Constant(ast::Const val_) : val(std::move(val_)) {}
  };
} // namespace tacky