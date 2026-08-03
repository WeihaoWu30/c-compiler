#pragma once
#include <list>
#include <array>
#include <regex>
#include <string>

namespace lexer
{
  extern std::array<std::regex, 53> patterns;
  std::list<std::string> lex(const std::string &filename);
}
