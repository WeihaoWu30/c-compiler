#pragma once
#include <list>

namespace lexer
{
  std::list<std::string> lex(const std::string &filename);
} // namespace lexer
