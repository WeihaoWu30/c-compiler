#pragma once
#include "ast/abstract/abstract.hpp"
#include "ast/block/block.hpp"
#include "ast/declarations/declarations.hpp"
#include "ast/identifier_attrs/identifier_attr.hpp"
#include "ast/top_level/program.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace semantic_analysis {
  struct MapEntry {
    std::string new_name;
    bool from_current_scope;
    bool has_linkage;
  };
  extern std::unordered_map<std::string, MapEntry> identifier_map; // formerly known as variable_map
  ast::Identifier* make_label();
  ast::Identifier* make_label(std::string label);
  std::string make_temporary(std::string s);
  std::unordered_map<std::string, MapEntry> copy_identifier_map(std::unordered_map<std::string, MapEntry>& identifier_map);
  std::shared_ptr<ast::Type> get_common_type(std::shared_ptr<ast::Type> type1, std::shared_ptr<ast::Type> type2);
  ast::Expression* convert_to(ast::Expression* e, std::shared_ptr<ast::Type> t, std::vector<std::unique_ptr<ast::Expression>>& expressions);
  void resolve_block(ast::Block* block, std::unordered_map<std::string, MapEntry>& identifier_map, bool is_file_scope);
  void resolve_exp(ast::Expression* e, std::unordered_map<std::string, MapEntry>& identifier_map);
  void resolve_declaration(ast::Declaration* declaration, std::unordered_map<std::string, MapEntry>& identifier_map, bool is_file_scope);
  void resolve_var_decl(ast::Var_Decl* var_decl, std::unordered_map<std::string, MapEntry>& identifier_map, bool is_file_scope);
  void resolve_fun_decl(ast::Fun_Decl* fun_decl, std::unordered_map<std::string, MapEntry>& identifier_map, bool is_file_scope);
  void resolve_params(ast::Identifier* identifier, std::unordered_map<std::string, MapEntry>& identifier_map);
  void resolve_statement(ast::Statement* statement, std::unordered_map<std::string, MapEntry>& identifier_map);
  void resolve_optional_exp(ast::Expression* exp, std::unordered_map<std::string, MapEntry>& identifier_map);
  void resolve_for_init(ast::For_Init* init, std::unordered_map<std::string, MapEntry>& identifier_map);
  ast::Statement* annotate(ast::Statement* statement, ast::Identifier* current_label);
  ast::Statement* label_statement(ast::Statement* statement, ast::Identifier* current_label);
  ast::Block* label_block(ast::Block* block, ast::Identifier* current_label);
  void typecheck_declaration(ast::Declaration* declaration, bool is_file_scope);
  void typecheck_file_scope_variable_declaration(ast::Var_Decl* var_decl);
  void typecheck_local_variable_declaration(ast::Var_Decl* var_decl);
  void typecheck_function_declaration(ast::Fun_Decl* fun_decl, std::vector<std::unique_ptr<ast::Expression>>& function_expressions);
  void typecheck_block(ast::Block* block, std::vector<std::unique_ptr<ast::Expression>>& function_expressions, bool is_file_scope);
  void typecheck_exp(ast::Expression* e, std::vector<std::unique_ptr<ast::Expression>>& function_expressions);
  void typecheck_statement(ast::Statement* statement, std::vector<std::unique_ptr<ast::Expression>>& function_expressions, bool is_file_scope, std::shared_ptr<ast::Type> return_type = nullptr);
  void analyze_program(ast::Program* program);
} // namespace semantic_analysis