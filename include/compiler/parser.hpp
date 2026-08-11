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
#include "ast/declarations/declarations.hpp"
#include "ast/block/block.hpp"
#include "ast/storage_class/storage_classes.hpp"
#include "ast/identifier_attrs/identifier_attr.hpp"
#include <memory>
#include <regex>
#include "compiler/symbols.hpp"

namespace parser
{
  static const std::regex naming_convention("^[a-zA-Z_][a-zA-Z0-9_]*$");
  constexpr std::array<std::string_view, 3> specifiers = {"static", "extern", "int"};
  constexpr std::array<std::string_view, 3> unary_operators = {"!", "~", "-"};
  constexpr std::array<std::string_view, 11> compound_operators = {"+=",
                                                                   "-=",
                                                                   "*=",
                                                                   "/=",
                                                                   "%=",
                                                                   "|=",
                                                                   "&=",
                                                                   "^=",
                                                                   ">>=",
                                                                   "<<="};
  constexpr std::array<std::string_view, 20> binary_operators = {"+",
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
                                                                 "^",
                                                                 "?"};
  struct MapEntry {
    std::string new_name;
    bool from_current_scope;
    bool has_linkage;
  };
  extern std::unordered_map<std::string, MapEntry> identifier_map; // formerly known as variable_map
  extern std::unordered_map<std::string, std::string> type_aliases; // maps types to typedef aliases
  extern uint32_t var_counter;
  void expect(std::string expected, std::list<std::string> &tokens);
  std::string make_temporary(std::string s);
  ast::Identifier *make_label();
  ast::Identifier *make_label(std::string label);
  std::unordered_map<std::string, MapEntry> copy_identifier_map(std::unordered_map<std::string, MapEntry> &identifier_map);
  ast::Unary_Operator parse_unop(std::list<std::string> &tokens);
  ast::Compound_Operator parse_comop(std::list<std::string> &tokens);
  ast::Binary_Operator parse_binop(std::list<std::string> &tokens);
  uint16_t precedence(const std::string &next_token);
  ast::Expression *parse_conditional_middle(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Expression *parse_factor(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Statement *parse_statement(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  ast::Declaration *parse_declaration(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions, std::pair<ast::Type *, ast::Storage_Class> &type_and_storage_class);
  std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions);
  void resolve_block(ast::Block *block, std::unordered_map<std::string, MapEntry> &identifier_map, bool is_file_scope);
  void resolve_exp(ast::Expression *e, std::unordered_map<std::string, MapEntry> &identifier_map);
  void resolve_declaration(ast::Declaration *declaration, std::unordered_map<std::string, MapEntry> &identifier_map, bool is_file_scope);
  void resolve_var_decl(ast::Var_Decl *var_decl, std::unordered_map<std::string, MapEntry> &identifier_map, bool is_file_scope);
  void resolve_fun_decl(ast::Fun_Decl *fun_decl, std::unordered_map<std::string, MapEntry> &identifier_map, bool is_file_scope);
  void resolve_params(ast::Identifier *identifier, std::unordered_map<std::string, MapEntry> &identifier_map);
  void resolve_statement(ast::Statement *statement, std::unordered_map<std::string, MapEntry> &identifier_map);
  void resolve_optional_exp(ast::Expression *exp, std::unordered_map<std::string, MapEntry> &identifier_map);
  void resolve_for_init(ast::For_Init *init, std::unordered_map<std::string, MapEntry> &identifier_map);
  ast::Statement *annotate(ast::Statement *statement, ast::Identifier *current_label);
  ast::Statement *label_statement(ast::Statement *statement, ast::Identifier *current_label);
  ast::Block *label_block(ast::Block *block, ast::Identifier *current_label);
  void typecheck_declaration(ast::Declaration *declaration, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> &symbols, bool is_file_scope);
  void typecheck_file_scope_variable_declaration(ast::Var_Decl *var_decl, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> &symbols);
  void typecheck_local_variable_declaration(ast::Var_Decl *var_decl, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> &symbols);
  void typecheck_function_declaration(ast::Fun_Decl *fun_decl, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> &symbols);
  void typecheck_block(ast::Block *block, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> &symbols, bool is_file_scope);
  void typecheck_exp(ast::Expression *e, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> &symbols);
  void typecheck_statement(ast::Statement *statement, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr>> &symbols, bool is_file_scope);
  void parse_parameters(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Identifier>> &params, const std::string &func_name);
  std::pair<ast::Type *, ast::Storage_Class> parse_type_and_storage_class(std::list<std::string> &tokens);
  ast::Program *parse_program(std::list<std::string> &tokens);
  ast::Program *parse(std::list<std::string> &tokens);
}
