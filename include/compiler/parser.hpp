#pragma once
#include <string>
#include <string_view>
#include <list>
#include <vector>
#include <array>
#include <unordered_map>
#include "ast/abstract/abstract.hpp"
#include "ast/operators/operators.hpp"
#include "ast/statements/return.hpp"
#include "ast/top_level/top_level.hpp"
#include <memory>
#include <utility>

namespace parser
{
  constexpr std::array<std::string_view, 19> binary_operators = {"+",
                                                                 "-",
                                                                 "/",
                                                                 "%",
                                                                 "*",
                                                                 "<",
                                                                 "<=",
                                                                 ">",
                                                                 ">=",
                                                                 "==",
                                                                 "!=",
                                                                 "&&",
                                                                 "||",
                                                                 "=",
                                                                 ">>",
                                                                 "<<",
                                                                 "&",
                                                                 "|",
                                                                 "^"};

  extern std::unordered_map<std::string, std::string> variable_map;
  extern std::unordered_map<std::string, std::string> symbol_table;
  void expect(std::string expected, std::list<std::string> &tokens);
  bool is_type(const std::string &token);
  ast::Unary_Operator parse_unop(std::list<std::string> &tokens);
  ast::Binary_Operator parse_binop(std::list<std::string> &tokens);
  uint16_t precedence(const std::string &next_token);
  ast::Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Expression *parse_factor(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Statement *parse_statement(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Expression *resolve_exp(ast::Expression *e, std::unordered_map<std::string, std::string> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Declaration *resolve_declaration(ast::Declaration *declaration, std::unordered_map<std::string, std::string> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Statement *resolve_statement(ast::Statement *statement, std::unordered_map<std::string, std::string> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Function *parse_function(std::list<std::string> &tokens);
  ast::Program *parse(std::list<std::string> &tokens);
}
