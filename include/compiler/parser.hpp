#pragma once
#include <string>
#include <list>
#include <unordered_set>
#include "ast/abstract/abstract.hpp"
#include "ast/statements/return.hpp"
#include "ast/top_level/top_level.hpp"
#include <memory>

namespace parser
{
  extern std::unordered_set<std::string> binary_operators;
  extern inline std::unordered_map<std::string, std::string> variable_map;
  void expect(std::string expected, std::list<std::string> &tokens);
  std::string make_temporary(std::string s);
  std::unique_ptr<ast::Unary_Operator> parse_unop(std::list<std::string> &tokens);
  std::unique_ptr<ast::Binary_Operator> parse_binop(std::list<std::string> &tokens);
  uint16_t precedence(const std::string &next_token);
  std::unique_ptr<ast::Expression> parse_expression(std::list<std::string> &tokens, uint16_t min_prec);
  std::unique_ptr<ast::Expression> parse_factor(std::list<std::string> &tokens);
  std::unique_ptr<ast::Statement> parse_statement(std::list<std::string> &tokens);
  std::unique_ptr<ast::Expression> resolve_exp(std::unique_ptr<ast::Expression> e);
  std::unique_ptr<ast::Declaration> resolve_declaration(std::unique_ptr<ast::Declaration> declaration);
  std::unique_ptr<ast::Statement> resolve_statement(std::unique_ptr<ast::Statement> statement);
  std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string> &tokens);
  std::unique_ptr<ast::Function> parse_function(std::list<std::string> &tokens);
  void pretty_print(std::unique_ptr<ast::Program> p);
  std::unique_ptr<ast::Program> parse(std::list<std::string> &tokens);
}
