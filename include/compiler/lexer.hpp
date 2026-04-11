#pragma once
#include <list>
#include <array>
#include <regex>

namespace lexer
{
  extern std::array<std::regex, 33> patterns;
  std::list<std::string> lex(const std::string &filename);
}
