#pragma once
#include <string>
#include <list>
#include <unordered_set>
#include "ast/abstract/abstract.hpp"
#include "ast/statements/return.hpp"
#include "ast/top_level/top_level.hpp"

namespace parser
{
  extern std::unordered_set<std::string> binary_operators;
  void expect(std::string expected, std::list<std::string> &tokens);
  ast::Unary_Operator *parse_unop(std::list<std::string> &tokens);
  ast::Binary_Operator *parse_binop(std::list<std::string> &tokens);
  uint16_t precedence(const std::string &next_token);
  ast::Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec);
  ast::Expression *parse_factor(std::list<std::string> &tokens);
  ast::Return *parse_statement(std::list<std::string> &tokens);
  ast::Function *parse_function(std::list<std::string> &tokens);
  void pretty_print(const ast::Program *p);
  ast::Program *parse(std::list<std::string> &tokens);
}
