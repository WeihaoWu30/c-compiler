#pragma once
#include "ast/abstract/block_item.hpp"
#include "ast/abstract/declaration.hpp"

namespace ast {
  struct D : Block_Item {
    Declaration* declaration;
    D(Declaration* declaration_) : declaration(declaration_){};
    ~D() { delete declaration; }
  };
} // namespace ast