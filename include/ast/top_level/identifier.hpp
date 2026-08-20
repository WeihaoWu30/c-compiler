#pragma once
#include <ostream>
#include <string>

namespace ast {
  struct Identifier {
    std::string text;
    Identifier(std::string text_) : text(text_) {}
  };
} // namespace ast
