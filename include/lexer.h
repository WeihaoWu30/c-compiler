#ifndef __LEXER_H__
#define __LEXER_H__
#include <list>

void trim(std::string &s);
std::list<std::string> lex(const std::string& filename);
#endif
