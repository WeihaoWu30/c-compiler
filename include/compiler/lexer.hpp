#pragma once
#include <list>
#include <vector>
#include <regex>

namespace lexer
{
  extern std::vector<std::regex> patterns;
  std::list<std::string> lex(const std::string &filename);
}
