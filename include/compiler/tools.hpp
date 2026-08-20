#pragma once
#include "ast/abstract/type.hpp"
#include "ast/identifier_attrs/identifier_attr.hpp"
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>

namespace tools {
  extern std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> symbols; // maps variable names to types
  extern uint32_t var_counter;
  static const std::regex naming_convention("^[a-zA-Z_][a-zA-Z0-9_]*$");
} // namespace tools