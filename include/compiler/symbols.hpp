#pragma once
#include <unordered_map>
#include "ast/abstract/type.hpp"
#include "ast/identifier_attrs/identifier_attr.hpp"
#include <string>
#include <memory>

namespace symbols
{
  extern std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> symbols; // maps variable names to types
}