#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>
#include <list>
#include "ast.h"

void expect(std::string expected, std::list<std::string>& tokens);
Constant* parse_expression(std::list<std::string> tokens);
Return* parse_statement(std::list<std::string> tokens);
Function* parse_function(std::list<std::string> tokens);
void pretty_print(const Program& p);
Program* parse(std::list<std::string>& tokens);

#endif
