#pragma once
#include <array>
#include <list>
#include <regex>
#include <string>

namespace lexer {
  extern std::array<std::regex, 55> patterns;
  std::list<std::string> lex(const std::string& filename);
} // namespace lexer
