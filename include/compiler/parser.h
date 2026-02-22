#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>
#include <list>
#include <unordered_set>
#include "ast.h"

extern std::unordered_set<std::string> binary_operators;

void expect(std::string expected, std::list<std::string>& tokens);
Unary_Operator *parse_unop(std::list<std::string> &tokens);
Binary_Operator *parse_binop(std::list<std::string> &tokens);
uint16_t precedence(const std::string& next_token);
Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec);
Expression *parse_factor(std::list<std::string> &tokens);
Return* parse_statement(std::list<std::string> &tokens);
Function* parse_function(std::list<std::string> &tokens);
void pretty_print(const Program *p);
Program* parse(std::list<std::string>& tokens);

#endif
