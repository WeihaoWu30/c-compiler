#include "compiler/semantic_analysis.hpp"
#include "ast/ast.hpp"
#include "ast/operators/unary_operators.hpp"
#include "ast/types/long.hpp"
#include "compiler/parser.hpp"
#include "compiler/tools.hpp"
#include <regex>
#include <cstddef>

namespace semantic_analysis {
  std::unordered_map<std::string, MapEntry> identifier_map;
  std::string make_temporary(std::string s) { return s + "." + std::to_string(tools::var_counter++); }
  ast::Identifier* make_label() { return make_label("loop." + std::to_string(tools::var_counter++)); }
  ast::Identifier* make_label(std::string label) { return new ast::Identifier(label); }
  std::unordered_map<std::string, MapEntry> copy_identifier_map(std::unordered_map<std::string, MapEntry>& identifier_map) {
    std::unordered_map<std::string, MapEntry> duplicate(identifier_map);
    for (auto& [identifier_name, map_entry] : duplicate) { map_entry.from_current_scope = false; }
    return duplicate;
  }
  std::shared_ptr<ast::Type> get_common_type(std::shared_ptr<ast::Type> type1, std::shared_ptr<ast::Type> type2) {
    if(!type1 || !type2) {
      return nullptr;
    }
    if (typeid(*type1) == typeid(*type2)) {
      return type1;
    } else {
      return std::make_shared<ast::Long>();
    }
  }
  ast::Expression* convert_to(ast::Expression* e, std::shared_ptr<ast::Type> t, std::vector<std::unique_ptr<ast::Expression>>& expressions) {
    if (e && t) {
      if (typeid(*e->type) == typeid(*t)) { return e; }
      std::unique_ptr<ast::Cast> cast_exp = std::make_unique<ast::Cast>(t, e);
      expressions.push_back(std::move(cast_exp));
      return expressions.back().get();
    }
    return nullptr;
  }
  void resolve_exp(ast::Expression* e, std::unordered_map<std::string, MapEntry>& identifier_map) {
    ast::Assignment* assignment = dynamic_cast<ast::Assignment*>(e);
    if (assignment) {
      ast::Var* var_node = dynamic_cast<ast::Var*>(assignment->lvalue);
      if (!var_node) { throw std::runtime_error("Invalid lvalue."); }
      resolve_exp(assignment->lvalue, identifier_map);
      resolve_exp(assignment->exp, identifier_map);
      return;
    }
    ast::Compound* compound = dynamic_cast<ast::Compound*>(e);
    if (compound) {
      ast::Var* var_node = dynamic_cast<ast::Var*>(compound->left);
      if (!var_node) { throw std::runtime_error("Invalid lvalue."); }
      resolve_exp(compound->left, identifier_map);
      resolve_exp(compound->right, identifier_map);
      return;
    }
    ast::Binary* binary = dynamic_cast<ast::Binary*>(e);
    if (binary) {
      resolve_exp(binary->left, identifier_map);
      resolve_exp(binary->right, identifier_map);
      return;
    }
    ast::Unary* unary = dynamic_cast<ast::Unary*>(e);
    if (unary) {
      resolve_exp(unary->exp, identifier_map);
      return;
    }
    ast::Var* var = dynamic_cast<ast::Var*>(e);
    if (var) {
      if (identifier_map.count(var->identifier->text)) {
        var->identifier->text = identifier_map[var->identifier->text].new_name;
        return;
      } else {
        throw std::runtime_error(std::format("{} is an undeclared variable.", var->identifier->text));
      }
    }
    ast::Conditional* conditional = dynamic_cast<ast::Conditional*>(e);
    if (conditional) {
      resolve_exp(conditional->condition, identifier_map);
      resolve_exp(conditional->left, identifier_map);
      resolve_exp(conditional->right, identifier_map);
      return;
    }
    ast::Function_Call* function_call = dynamic_cast<ast::Function_Call*>(e);
    if (function_call) {
      if (identifier_map.count(function_call->identifier->text)) {
        function_call->identifier->text = identifier_map[function_call->identifier->text].new_name;
        for (auto& arg : function_call->args) { resolve_exp(arg, identifier_map); }
        return;
      } else {
        throw std::runtime_error(std::format("{} is an undeclared function.", function_call->identifier->text));
      }
    }
  }

  void resolve_block(ast::Block* block, std::unordered_map<std::string, MapEntry>& identifier_map, bool is_file_scope) {
    for (auto& item : block->block_items) {
      ast::D* d = dynamic_cast<ast::D*>(item.get());
      ast::S* s = dynamic_cast<ast::S*>(item.get());

      if (d) {
        resolve_declaration(d->declaration, identifier_map, is_file_scope);
      } else if (s) {
        resolve_statement(s->statement, identifier_map);
      }
    }
  }

  void resolve_declaration(ast::Declaration* declaration, std::unordered_map<std::string, MapEntry>& identifier_map, bool is_file_scope) {
    ast::Var_Decl* var_decl = dynamic_cast<ast::Var_Decl*>(declaration);
    ast::Fun_Decl* fun_decl = dynamic_cast<ast::Fun_Decl*>(declaration);
    if (var_decl) { resolve_var_decl(var_decl, identifier_map, is_file_scope); }
    if (fun_decl) { resolve_fun_decl(fun_decl, identifier_map, is_file_scope); }
  }

  void resolve_var_decl(ast::Var_Decl* var_decl, std::unordered_map<std::string, MapEntry>& identifier_map, bool is_file_scope) {
    if (!std::regex_match(var_decl->name->text, tools::naming_convention)) { throw std::runtime_error(std::format("{} is not a valid name for a variable.", var_decl->name->text)); }
    std::string variable_name = var_decl->name->text;
    if (!is_file_scope) {
      if (auto it = identifier_map.find(variable_name); it != identifier_map.end()) {
        MapEntry& prev_entry = it->second;
        if (prev_entry.from_current_scope) {
          if (!(prev_entry.has_linkage && var_decl->storage_class == ast::Storage_Class::EXTERN)) {
            throw std::runtime_error(std::format("{} has already been declared in this scope.", variable_name));
          }
        }
      }

      if (var_decl->storage_class == ast::Storage_Class::EXTERN) {
        identifier_map.insert_or_assign(variable_name, MapEntry{variable_name, true, true});
      } else {
        std::string unique_name = make_temporary(variable_name);
        identifier_map.insert_or_assign(variable_name, MapEntry{unique_name, true, false});
        var_decl->name->text = unique_name;
        if (var_decl->init) { resolve_exp(var_decl->init, identifier_map); }
      }
    } else {
      identifier_map.insert_or_assign(variable_name, MapEntry{variable_name, true, is_file_scope});
    }
  }

  void resolve_fun_decl(ast::Fun_Decl* fun_decl, std::unordered_map<std::string, MapEntry>& identifier_map, bool is_file_scope) {
    std::string function_name = fun_decl->name->text;
    if (!std::regex_match(function_name, tools::naming_convention)) { throw std::runtime_error(std::format("{} is not a valid name for a function.", function_name)); }
    if (!is_file_scope && fun_decl->storage_class == ast::Storage_Class::STATIC) { throw std::runtime_error("Static functions are not supported in this scope."); }
    if (auto it = identifier_map.find(function_name); it != identifier_map.end()) {
      MapEntry& prev_entry = it->second;
      if (prev_entry.from_current_scope && !prev_entry.has_linkage) { throw std::runtime_error(std::format("The function {} has already been declared.", function_name)); }
    }

    identifier_map.insert_or_assign(function_name, MapEntry{function_name, true, true});

    std::unordered_map<std::string, MapEntry> inner_map = copy_identifier_map(identifier_map);
    for (auto& param : fun_decl->params) { resolve_params(param.get(), inner_map); }

    if (fun_decl->body) {
      if (!is_file_scope) { throw std::runtime_error("Function definitions are not supported in this scope."); }
      resolve_block(fun_decl->body, inner_map, false);
      fun_decl->body = label_block(fun_decl->body, nullptr); // labels own block after resolving it for a fully complete ast node
    }
  }

  void resolve_params(ast::Identifier* identifier, std::unordered_map<std::string, MapEntry>& identifier_map) {
    if (!std::regex_match(identifier->text, tools::naming_convention)) { throw std::runtime_error(std::format("{} is not a valid name for a parameter.", identifier->text)); }
    if (identifier_map.count(identifier->text) && identifier_map[identifier->text].from_current_scope) {
      throw std::runtime_error(std::format("The parameter {} has already been declared.", identifier->text));
    }

    std::string unique_name = make_temporary(identifier->text);
    identifier_map.insert_or_assign(identifier->text, MapEntry{unique_name, true, false});
    identifier->text = unique_name;
  }

  void resolve_statement(ast::Statement* statement, std::unordered_map<std::string, MapEntry>& identifier_map) {
    ast::Return* ret = dynamic_cast<ast::Return*>(statement);
    if (ret) {
      resolve_exp(ret->exp, identifier_map);
      return;
    }
    ast::Expression_Statement* exp_statement = dynamic_cast<ast::Expression_Statement*>(statement);
    if (exp_statement) {
      resolve_exp(exp_statement->exp, identifier_map);
      return;
    }
    ast::If* if_statement = dynamic_cast<ast::If*>(statement);
    if (if_statement) {
      resolve_exp(if_statement->condition, identifier_map);
      resolve_statement(if_statement->then_statement, identifier_map);
      if (if_statement->else_statement) { resolve_statement(if_statement->else_statement, identifier_map); }
      return;
    }
    ast::Compound_Statement* compound_statement = dynamic_cast<ast::Compound_Statement*>(statement);
    if (compound_statement) {
      std::unordered_map<std::string, MapEntry> new_identifier_map = copy_identifier_map(identifier_map);
      resolve_block(compound_statement->block, new_identifier_map, false);
      return;
    }
    std::string dummy_text;
    ast::Break* break_statement = dynamic_cast<ast::Break*>(statement);
    if (break_statement) {
      dummy_text = break_statement->label->text;
      delete break_statement->label;
      break_statement->label = make_label(dummy_text);
      return;
    }
    ast::Continue* continue_statement = dynamic_cast<ast::Continue*>(statement);
    if (continue_statement) {
      dummy_text = continue_statement->label->text;
      delete continue_statement->label;
      continue_statement->label = make_label(dummy_text);
      return;
    }
    ast::For* for_statement = dynamic_cast<ast::For*>(statement);
    if (for_statement) {
      std::unordered_map<std::string, MapEntry> new_identifier_map = copy_identifier_map(identifier_map);
      resolve_for_init(for_statement->init, new_identifier_map);
      resolve_optional_exp(for_statement->condition, new_identifier_map);
      resolve_optional_exp(for_statement->post, new_identifier_map);
      resolve_statement(for_statement->body, new_identifier_map);
      dummy_text = for_statement->label->text;
      delete for_statement->label;
      for_statement->label = make_label(dummy_text);
      return;
    }
    ast::While* while_statement = dynamic_cast<ast::While*>(statement);
    if (while_statement) {
      resolve_exp(while_statement->condition, identifier_map);
      resolve_statement(while_statement->body, identifier_map);
      dummy_text = while_statement->label->text;
      delete while_statement->label;
      while_statement->label = make_label(dummy_text);
      return;
    }
    ast::DoWhile* do_while_statement = dynamic_cast<ast::DoWhile*>(statement);
    if (do_while_statement) {
      resolve_statement(do_while_statement->body, identifier_map);
      resolve_exp(do_while_statement->condition, identifier_map);
      dummy_text = do_while_statement->label->text;
      delete do_while_statement->label;
      do_while_statement->label = make_label(dummy_text);
      return;
    }
  }

  void resolve_for_init(ast::For_Init* init, std::unordered_map<std::string, MapEntry>& identifier_map) {
    ast::Init_Decl* init_decl = dynamic_cast<ast::Init_Decl*>(init);
    ast::Init_Exp* init_exp = dynamic_cast<ast::Init_Exp*>(init);
    if (init_decl) {
      if (init_decl->variable_declaration->storage_class != ast::Storage_Class::NONE) { throw std::runtime_error("Declarations in for loops must not contain specifiers."); }
      resolve_declaration(init_decl->variable_declaration, identifier_map, false);
    } else if (init_exp) {
      resolve_optional_exp(init_exp->expression, identifier_map);
    }
  }

  void resolve_optional_exp(ast::Expression* exp, std::unordered_map<std::string, MapEntry>& identifier_map) {
    if (exp) { resolve_exp(exp, identifier_map); }
  }

  ast::Statement* annotate(ast::Statement* statement, ast::Identifier* current_label) {
    ast::Break* break_statement = dynamic_cast<ast::Break*>(statement);
    if (break_statement) {
      delete break_statement->label;
      break_statement->label = current_label;
      return break_statement;
    }
    ast::Continue* continue_statement = dynamic_cast<ast::Continue*>(statement);
    if (continue_statement) {
      delete continue_statement->label;
      continue_statement->label = current_label;
      return continue_statement;
    }
    ast::While* while_statement = dynamic_cast<ast::While*>(statement);
    if (while_statement) {
      delete while_statement->label;
      while_statement->label = current_label;
      return while_statement;
    }
    ast::DoWhile* do_while_statement = dynamic_cast<ast::DoWhile*>(statement);
    if (do_while_statement) {
      delete do_while_statement->label;
      do_while_statement->label = current_label;
      return do_while_statement;
    }
    ast::For* for_statement = dynamic_cast<ast::For*>(statement);
    if (for_statement) {
      delete for_statement->label;
      for_statement->label = current_label;
      return for_statement;
    }
    return statement;
  }

  // ugly, but faster returns
  ast::Statement* label_statement(ast::Statement* statement, ast::Identifier* current_label) {
    ast::Break* break_statement = dynamic_cast<ast::Break*>(statement);
    if (break_statement) {
      if (!current_label) { throw std::runtime_error("Break statement outside of loop."); }
      ast::Identifier* copy = make_label(current_label->text);
      return annotate(break_statement, copy);
    }
    ast::Continue* continue_statement = dynamic_cast<ast::Continue*>(statement);
    if (continue_statement) {
      if (!current_label) { throw std::runtime_error("Continue statement outside of loop."); }
      ast::Identifier* copy = make_label(current_label->text);
      return annotate(continue_statement, copy);
    }
    ast::Compound_Statement* compound_statement = dynamic_cast<ast::Compound_Statement*>(statement);
    if (compound_statement) {
      compound_statement->block = label_block(compound_statement->block, current_label);
      return compound_statement;
    }
    ast::If* if_statement = dynamic_cast<ast::If*>(statement);
    if (if_statement) {
      if_statement->then_statement = label_statement(if_statement->then_statement, current_label);
      if (if_statement->else_statement) { if_statement->else_statement = label_statement(if_statement->else_statement, current_label); }
      return if_statement;
    }
    ast::Expression_Statement* expression_statement = dynamic_cast<ast::Expression_Statement*>(statement);
    ast::Return* return_statement = dynamic_cast<ast::Return*>(statement);
    ast::Null* null = dynamic_cast<ast::Null*>(statement);
    if (expression_statement || return_statement || null) { return statement; }
    ast::While* while_statement = dynamic_cast<ast::While*>(statement);
    ast::DoWhile* do_while_statement = dynamic_cast<ast::DoWhile*>(statement);
    ast::For* for_statement = dynamic_cast<ast::For*>(statement);
    if (while_statement || do_while_statement || for_statement) {
      ast::Identifier* new_label = make_label();
      if (while_statement) {
        while_statement->body = label_statement(while_statement->body, new_label);
        return annotate(while_statement, new_label);
      } else if (do_while_statement) {
        do_while_statement->body = label_statement(do_while_statement->body, new_label);
        return annotate(do_while_statement, new_label);
      } else if (for_statement) {
        for_statement->body = label_statement(for_statement->body, new_label);
        return annotate(for_statement, new_label);
      }
    }
    return statement;
  }

  ast::Block* label_block(ast::Block* block, ast::Identifier* current_label) {
    for (auto& item : block->block_items) {
      ast::S* s = dynamic_cast<ast::S*>(item.get());
      if (s) { s->statement = label_statement(s->statement, current_label); }
    }
    return block;
  }

  void typecheck_declaration(ast::Declaration* declaration, bool is_file_scope) {
    ast::Var_Decl* var_decl = dynamic_cast<ast::Var_Decl*>(declaration);
    ast::Fun_Decl* fun_decl = dynamic_cast<ast::Fun_Decl*>(declaration);
    if (var_decl) {
      if (is_file_scope) {
        typecheck_file_scope_variable_declaration(var_decl);
      } else {
        typecheck_local_variable_declaration(var_decl);
      }
    } else if (fun_decl) {
      typecheck_function_declaration(fun_decl, fun_decl->expressions); // top level function, we pass down its expressions container
    } else {
      throw std::runtime_error("Invalid declaration.");
    }
  }

  void typecheck_file_scope_variable_declaration(ast::Var_Decl* var_decl) {
    ast::Constant* constant = dynamic_cast<ast::Constant*>(var_decl->init);
    ast::Initial_Value initial_value;
    if (constant) {
      initial_value = ast::Initial(constant->val);
    } else if (!var_decl->init) {
      if (var_decl->storage_class == ast::Storage_Class::EXTERN) {
        initial_value = ast::No_Initializer();
      } else {
        initial_value = ast::Tentative();
      }
    } else {
      throw std::runtime_error(std::format("Variable declaration {} has a non-constant initializer.", var_decl->name->text));
    }

    bool global = var_decl->storage_class != ast::Storage_Class::STATIC;

    if (auto it = tools::symbols.find(var_decl->name->text); it != tools::symbols.end()) {
      std::pair<std::shared_ptr<ast::Type>, ast::Identifier_Attr>& old_decl = it->second;
      ast::Int* old_int_type = dynamic_cast<ast::Int*>(old_decl.first.get());
      if (!old_int_type) { throw std::runtime_error(std::format("Function {} is being redeclared as a variable.", var_decl->name->text)); }
      if (!std::holds_alternative<ast::Static_Attr>(old_decl.second)) { throw std::runtime_error(std::format("Variable {} is declared as local in file scope.", var_decl->name->text)); }
      ast::Static_Attr old_decl_attr = std::get<ast::Static_Attr>(old_decl.second);
      if (var_decl->storage_class == ast::Storage_Class::EXTERN) {
        global = old_decl_attr.global;
      } else if (old_decl_attr.global != global) {
        throw std::runtime_error(std::format("Variable {} has conflicting variable linkage.", var_decl->name->text));
      }
      if (std::holds_alternative<ast::Initial>(old_decl_attr.init)) {
        if (std::holds_alternative<ast::Initial>(initial_value)) {
          throw std::runtime_error(std::format("Variable {} has conflicting file scope variable definitions.", var_decl->name->text));
        } else {
          initial_value = old_decl_attr.init;
        }
      } else if (!std::holds_alternative<ast::Initial>(initial_value) && std::holds_alternative<ast::Tentative>(old_decl_attr.init)) {
        initial_value = ast::Tentative();
      }
    }

    ast::Identifier_Attr new_decl_attr = ast::Static_Attr(initial_value, global);
    tools::symbols.insert_or_assign(var_decl->name->text, std::make_pair(std::make_shared<ast::Int>(), new_decl_attr));
    // we don't need to typecheck init since it can only be constant or empty as a file scope variable
  }

  void typecheck_local_variable_declaration(ast::Var_Decl* var_decl) {
    if (var_decl->storage_class == ast::Storage_Class::EXTERN) {
      if (var_decl->init) { throw std::runtime_error(std::format("Local extern variable declaration {} has an initializer.", var_decl->name->text)); }
      if (auto it = tools::symbols.find(var_decl->name->text); it != tools::symbols.end()) {
        std::pair<std::shared_ptr<ast::Type>, ast::Identifier_Attr>& old_decl = it->second;
        ast::Int* old_int_type = dynamic_cast<ast::Int*>(old_decl.first.get());
        if (!old_int_type) { throw std::runtime_error(std::format("Function {} is being redeclared as a variable.", var_decl->name->text)); }
      } else {
        ast::Identifier_Attr new_decl_attr = ast::Static_Attr(ast::No_Initializer(), true);
        tools::symbols.insert_or_assign(var_decl->name->text, std::make_pair(std::make_shared<ast::Int>(), new_decl_attr));
      }
    } else if (var_decl->storage_class == ast::Storage_Class::STATIC) {
      ast::Initial_Value initial_value;
      ast::Constant* constant = dynamic_cast<ast::Constant*>(var_decl->init);
      if (constant) {
        initial_value = ast::Initial(constant->val);
      } else if (!var_decl->init) {
        initial_value = ast::Initial(0);
      } else {
        throw std::runtime_error(std::format("Local static variable declaration {} has a non-constant initializer.", var_decl->name->text));
      }
      ast::Identifier_Attr new_decl_attr = ast::Static_Attr(initial_value, false);
      tools::symbols.insert_or_assign(var_decl->name->text, std::make_pair(std::make_shared<ast::Int>(), new_decl_attr));
    } else {
      ast::Identifier_Attr new_decl_attr = ast::Local_Attr();
      tools::symbols.insert_or_assign(var_decl->name->text, std::make_pair(std::make_shared<ast::Int>(), new_decl_attr));
      if (var_decl->init) { typecheck_exp(var_decl->init); }
    }
  }

  // fix this tmmr
  void typecheck_function_declaration(ast::Fun_Decl* fun_decl, std::vector<std::unique_ptr<ast::Expression>>& function_expressions) {
    std::unique_ptr<ast::Fun_Type> fun_type = std::make_unique<ast::Fun_Type>(fun_decl->params.size());
    ast::Block* body = fun_decl->body;
    bool already_defined = false;
    bool global = fun_decl->storage_class != ast::Storage_Class::STATIC;

    if (auto it = tools::symbols.find(fun_decl->name->text); it != tools::symbols.end()) {
      std::pair<std::shared_ptr<ast::Type>, ast::Identifier_Attr>& old_decl = it->second;
      ast::Fun_Type* old_fun_type = dynamic_cast<ast::Fun_Type*>(old_decl.first.get());
      if (!old_fun_type || old_fun_type->param_types.size() != fun_type->param_types.size()) { throw std::runtime_error("Incompatible function declarations."); }
      if (!std::holds_alternative<ast::Fun_Attr>(old_decl.second)) { throw std::runtime_error(std::format("Variable {} is being redeclared as a function.", fun_decl->name->text)); }
      ast::Fun_Attr old_decl_attr = std::get<ast::Fun_Attr>(old_decl.second); // if its a function declaration, it will be a Fun_Attr
      already_defined = old_decl_attr.defined;
      if (already_defined && body) { throw std::runtime_error("Function is defined more than once."); }
      if (old_decl_attr.global && fun_decl->storage_class == ast::Storage_Class::STATIC) {
        throw std::runtime_error(std::format("Static function declaration {} follows non-static.", fun_decl->name->text));
      }
      global = old_decl_attr.global;
    }
    ast::Identifier_Attr fun_attr = ast::Fun_Attr(already_defined || body, global);
    tools::symbols.insert_or_assign(fun_decl->name->text, std::make_pair(std::move(fun_type), fun_attr));
    if (body) {
      for (auto& param : fun_decl->params) { tools::symbols.emplace(param->text, std::make_pair(std::make_shared<ast::Int>(), ast::Local_Attr())); }
      typecheck_block(body, function_expressions, false);
    }
  }

  // add type checking for statements later
  void typecheck_block(ast::Block* block, std::vector<std::unique_ptr<ast::Expression>>& function_expressions, bool is_file_scope, std::shared_ptr<ast::Type> return_type = nullptr) {
    for (auto& item : block->block_items) {
      ast::D* d = dynamic_cast<ast::D*>(item.get());
      if (d) {
        if (dynamic_cast<ast::Var_Decl*>(d->declaration)) {
          if (is_file_scope) {
            typecheck_file_scope_variable_declaration(dynamic_cast<ast::Var_Decl*>(d->declaration));
          } else {
            typecheck_local_variable_declaration(dynamic_cast<ast::Var_Decl*>(d->declaration));
          }
        } else if (dynamic_cast<ast::Fun_Decl*>(d->declaration)) {
          // this will always be a declaration without a body so there is no expressions it contains for us to pass down so we pass down the top level function_expressions container
          typecheck_function_declaration(dynamic_cast<ast::Fun_Decl*>(d->declaration), function_expressions); 
        }
      }
      ast::S* s = dynamic_cast<ast::S*>(item.get());
      if (s) { typecheck_statement(s->statement, function_expressions, is_file_scope, return_type); }
    }
  }

  // add type checking for every other expression type later
  void typecheck_exp(ast::Expression* e, std::vector<std::unique_ptr<ast::Expression>>& function_expressions) {
    ast::Function_Call* function_call = dynamic_cast<ast::Function_Call*>(e);
    if (function_call) {
      auto it = tools::symbols.find(function_call->identifier->text);
      if (it == tools::symbols.end()) { throw std::runtime_error(std::format("Function {} not defined or declared in scope.", function_call->identifier->text)); }
      ast::Fun_Type* f_type = dynamic_cast<ast::Fun_Type*>(it->second.first.get());
      if (!f_type) { throw std::runtime_error(std::format("Variable {} used as function name.", function_call->identifier->text)); }
      if (f_type->param_types.size() != function_call->args.size()) {
        throw std::runtime_error(std::format("Function {} takes {} arguments, but {} were provided.", function_call->identifier->text, f_type->param_types.size(), function_call->args.size()));
      }
      for (std::size_t i{}; i < f_type->param_types.size(); ++i) {
        typecheck_exp(function_call->args[i], function_expressions);
        function_call->args[i] = convert_to(function_call->args[i], f_type->param_types[i], function_expressions);
      }
      function_call->type = f_type->return_type;
      return;
    }
    ast::Var* var = dynamic_cast<ast::Var*>(e);
    if (var) {
      if (auto it = tools::symbols.find(var->identifier->text); it != tools::symbols.end()) {
        std::pair<std::shared_ptr<ast::Type>, ast::Identifier_Attr>& var_type = it->second;
        if (dynamic_cast<ast::Fun_Type*>(var_type.first.get())) { throw std::runtime_error(std::format("Function name {} used as variable.", var->identifier->text)); }
        var->type = var_type.first;
        return;
      } else {
        throw std::runtime_error(std::format("Variable {} not defined or declared in scope.", var->identifier->text));
      }
    }
    ast::Constant* constant = dynamic_cast<ast::Constant*>(e);
    if (constant) {
      ast::Const constant_type = constant->const_type;
      if (std::holds_alternative<ast::ConstInt>(constant_type)) {
        constant->type = std::make_shared<ast::Int>();
      } else if (std::holds_alternative<ast::ConstLong>(constant_type)) {
        constant->type = std::make_shared<ast::Long>();
      } else {
        throw std::runtime_error("Invalid constant type declared in expression.");
      }
      return;
    }
    ast::Cast* cast = dynamic_cast<ast::Cast*>(e);
    if (cast) {
      typecheck_exp(cast->expression, function_expressions);
      return;
    }
    ast::Assignment* assignment = dynamic_cast<ast::Assignment*>(e);
    if (assignment) {
      typecheck_exp(assignment->lvalue, function_expressions);
      typecheck_exp(assignment->exp, function_expressions);
      assignment->exp = convert_to(assignment->exp, assignment->lvalue->type, function_expressions);
      assignment->type = assignment->lvalue->type;
      return;
    }
    ast::Binary* binary = dynamic_cast<ast::Binary*>(e);
    if (binary) {
      typecheck_exp(binary->left, function_expressions);
      typecheck_exp(binary->right, function_expressions);
      if (binary->binary_operator == ast::Binary_Operator::And || binary->binary_operator == ast::Binary_Operator::Or) {
        binary->type = std::make_shared<ast::Int>();
        return;
      }
      std::shared_ptr<ast::Type> common_type = get_common_type(binary->left->type, binary->right->type);
      binary->left = convert_to(binary->left, common_type, function_expressions);
      binary->right = convert_to(binary->right, common_type, function_expressions);
      if (binary->binary_operator == ast::Binary_Operator::Add ||
          binary->binary_operator == ast::Binary_Operator::Subtract ||
          binary->binary_operator == ast::Binary_Operator::Multiply ||
          binary->binary_operator == ast::Binary_Operator::Divide ||
          binary->binary_operator == ast::Binary_Operator::Remainder) {
        binary->type = common_type;
      } else {
        binary->type = std::make_shared<ast::Int>();
      }
      return;
    }
    ast::Compound* compound = dynamic_cast<ast::Compound*>(e);
    if (compound) {
      typecheck_exp(compound->left, function_expressions);
      typecheck_exp(compound->right, function_expressions);
      compound->right = convert_to(compound->right, compound->left->type, function_expressions);
      compound->type = compound->left->type;
      return;
    }
    ast::Conditional* conditional = dynamic_cast<ast::Conditional*>(e);
    if (conditional) {
      typecheck_exp(conditional->condition, function_expressions);
      typecheck_exp(conditional->left, function_expressions);
      typecheck_exp(conditional->right, function_expressions);
      std::shared_ptr<ast::Type> common_type = get_common_type(conditional->left->type, conditional->right->type);
      conditional->left = convert_to(conditional->left, common_type, function_expressions);
      conditional->right = convert_to(conditional->right, common_type, function_expressions);
      conditional->type = common_type;
      return;
    }
    ast::Unary* unary = dynamic_cast<ast::Unary*>(e);
    if (unary) {
      typecheck_exp(unary->exp, function_expressions);
      if (unary->unary_operator == ast::Unary_Operator::Not) {
        unary->type = std::make_shared<ast::Int>();
      } else {
        unary->type = unary->exp->type;
      }
      return;
    }
  }

  void typecheck_statement(ast::Statement* statement, std::vector<std::unique_ptr<ast::Expression>>& function_expressions, bool is_file_scope, std::shared_ptr<ast::Type> return_type = nullptr) {
    ast::Expression_Statement* expression_statement = dynamic_cast<ast::Expression_Statement*>(statement);
    if (expression_statement) {
      typecheck_exp(expression_statement->exp, function_expressions);
      return;
    }
    ast::Return* return_statement = dynamic_cast<ast::Return*>(statement);
    if (return_statement) {
      typecheck_exp(return_statement->exp, function_expressions);
      return_statement->exp = convert_to(return_statement->exp, return_type, function_expressions);
      return;
    }
    ast::If* if_statement = dynamic_cast<ast::If*>(statement);
    if (if_statement) {
      typecheck_exp(if_statement->condition, function_expressions);
      typecheck_statement(if_statement->then_statement, function_expressions, is_file_scope, return_type);
      if (if_statement->else_statement) { typecheck_statement(if_statement->else_statement, function_expressions, is_file_scope); }
      return;
    }
    ast::Compound_Statement* compound_statement = dynamic_cast<ast::Compound_Statement*>(statement);
    if (compound_statement) {
      typecheck_block(compound_statement->block, function_expressions, is_file_scope, return_type);
      return;
    }
    ast::For* for_statement = dynamic_cast<ast::For*>(statement);
    if (for_statement) {
      ast::Init_Decl* init_decl = dynamic_cast<ast::Init_Decl*>(for_statement->init);
      if (init_decl) { typecheck_local_variable_declaration(dynamic_cast<ast::Var_Decl*>(init_decl->variable_declaration)); }
      ast::Init_Exp* init_exp = dynamic_cast<ast::Init_Exp*>(for_statement->init);
      if (init_exp && init_exp->expression) { typecheck_exp(init_exp->expression, function_expressions); }
      if (for_statement->condition) { typecheck_exp(for_statement->condition, function_expressions); }
      if (for_statement->post) { typecheck_exp(for_statement->post, function_expressions); }
      typecheck_statement(for_statement->body, function_expressions, is_file_scope, return_type);
      return;
    }
    ast::While* while_statement = dynamic_cast<ast::While*>(statement);
    if (while_statement) {
      typecheck_exp(while_statement->condition, function_expressions);
      typecheck_statement(while_statement->body, function_expressions, is_file_scope, return_type);
      return;
    }
    ast::DoWhile* do_while_statement = dynamic_cast<ast::DoWhile*>(statement);
    if (do_while_statement) {
      typecheck_statement(do_while_statement->body, function_expressions, is_file_scope, return_type);
      typecheck_exp(do_while_statement->condition, function_expressions);
      return;
    }
  }

  void analyze_program(ast::Program* program) {
    for (auto& item : program->declarations) {
      resolve_declaration(item.get(), identifier_map, true);
      typecheck_declaration(item.get(), true);
    }
    identifier_map.clear();
  }
} // namespace semantic_analysis