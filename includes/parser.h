#pragma once
#include <iostream>
#include <string>
#include <list>

class Constant;
class Return;
class Function;
class Program;

void expect(std::string expected, std::list<std::string>& tokens);
Constant* parse_expression(std::list<std::string> tokens);
Return* parse_statement(std::list<std::string> tokens);
Function* parse_function(std::list<std::string> tokens);
void pretty_print(const Program& p);