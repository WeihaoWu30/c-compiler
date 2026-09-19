#pragma once
#include "ast/abstract/abstract.hpp"
#include "ast/declarations/declarations.hpp"
#include "ast/operators/operators.hpp"
#include "ast/storage_class/storage_classes.hpp"
#include "ast/constants/const.hpp"
#include "ast/top_level/top_level.hpp"
#include "compiler/tools.hpp"
#include <array>
#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <regex>

namespace parser {
  constexpr std::array<std::string_view, 2> specifiers = {"static", "extern"};
  constexpr std::array<std::string_view, 2> type_specifiers = {"int", "long"};
  constexpr std::array<std::string_view, 3> unary_operators = {"!", "~", "-"};
  constexpr std::array<std::string_view, 11> compound_operators = {"+=", "-=", "*=", "/=", "%=", "|=", "&=", "^=", ">>=", "<<="};
  constexpr std::array<std::string_view, 20> binary_operators = {"+", "-", "/", "%", "*", "<", "<=", ">", ">=", "==", "!=", "&&", "||", "=", ">>", "<<", "&", "|", "^", "?"};
  static const std::regex const_regex = std::regex(R"([0-9]+[lL]?)");
  extern std::unordered_map<std::string, std::string> type_aliases; // maps types to typedef aliases
  void expect(std::string expected, std::list<std::string>& tokens);
  ast::Unary_Operator parse_unop(std::list<std::string>& tokens);
  ast::Compound_Operator parse_comop(std::list<std::string>& tokens);
  ast::Binary_Operator parse_binop(std::list<std::string>& tokens);
  uint16_t precedence(const std::string& next_token);
  ast::Expression* parse_conditional_middle(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions);
  ast::Expression* parse_expression(std::list<std::string>& tokens, uint16_t min_prec, std::vector<std::unique_ptr<ast::Expression>>& expressions);
  ast::Const parse_Constant(std::string& token);
  ast::Expression* parse_factor(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions);
  ast::Statement* parse_statement(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions);
  ast::Declaration* parse_declaration(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions, std::pair<std::unique_ptr<ast::Type>, ast::Storage_Class>& type_and_storage_class);
  std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions);
  void parse_parameters(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Identifier>>& params, const std::string& func_name);
  std::unique_ptr<ast::Type> parse_type(std::vector<std::string>& specifier_list);
  std::pair<std::unique_ptr<ast::Type>, ast::Storage_Class> parse_type_and_storage_class(std::list<std::string>& tokens);
  ast::Program* parse_program(std::list<std::string>& tokens);
  ast::Program* parse(std::list<std::string>& tokens);
} // namespace parser
