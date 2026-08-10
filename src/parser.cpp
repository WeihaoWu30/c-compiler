#include "ast/abstract/declaration.hpp"
#include "ast/ast.hpp"
#include "ast/declarations/var_decl.hpp"
#include "compiler/parser.hpp"
#include <stdexcept>
#include <format>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <utility>
#include <iterator>
#include <regex>

// This File is meant to convert the tokens into Abstract Syntax Tree nodes

namespace parser
{
   // For Parsing Expressions
   std::unordered_map<std::string, MapEntry> identifier_map; // formerly known as variable_map
   std::unordered_map<std::string, std::string> type_aliases; // maps types to typedef aliases
   std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, bool>> symbols; // maps variable names to types
   uint32_t var_counter = 0;

   // This Function Matches A Token Against Legal Syntax
   void expect(std::string expected, std::list<std::string> &tokens)
   {
      if (tokens.empty())
      {
         throw std::runtime_error(std::format("Expected {}, but found nothing.", expected));
      }
      std::string actual(tokens.front());
      if (actual == expected)
      {
         tokens.pop_front();
      }
      else
      {
         throw std::runtime_error(std::format("Expected {} but found {}.", expected, actual));
      }
   }

   std::string make_temporary(std::string s)
   {
      return s + "." + std::to_string(var_counter++);
   }

   ast::Identifier *make_label() {
      return make_label("loop." + std::to_string(var_counter++));
   }

   ast::Identifier *make_label(std::string label) {
      return new ast::Identifier(label);
   }

   std::unordered_map<std::string, MapEntry> copy_identifier_map(std::unordered_map<std::string, MapEntry> &identifier_map) {
      std::unordered_map<std::string, MapEntry> duplicate(identifier_map);
      for ( auto &[identifier_name, map_entry] : duplicate) {
         map_entry.from_current_scope = false;
      }
      return duplicate;
   }


   // This function ONLY matches the next token against a bitwise negate or bitwise complement
   ast::Unary_Operator parse_unop(std::list<std::string> &tokens)
   {
      std::string next_token(tokens.front());
      tokens.pop_front();
      if (next_token == "~")
         return ast::Unary_Operator::Complement;
      else if (next_token == "-")
         return ast::Unary_Operator::Negate;
      else if (next_token == "!")
         return ast::Unary_Operator::Not;
      return ast::Unary_Operator::Invalid;
   }

   // This function creates AST nodes for binary operators
   ast::Binary_Operator parse_binop(std::list<std::string> &tokens)
   {
      std::string next_token(tokens.front());
      tokens.pop_front();
      if (next_token == "+")
         return ast::Binary_Operator::Add;
      else if (next_token == "-")
         return ast::Binary_Operator::Subtract;
      else if (next_token == "/")
         return ast::Binary_Operator::Divide;
      else if (next_token == "*")
         return ast::Binary_Operator::Multiply;
      else if (next_token == "%")
         return ast::Binary_Operator::Remainder;
      else if (next_token == "<")
         return ast::Binary_Operator::LessThan;
      else if (next_token == "<=")
         return ast::Binary_Operator::LessOrEqual;
      else if (next_token == ">")
         return ast::Binary_Operator::GreaterThan;
      else if (next_token == ">=")
         return ast::Binary_Operator::GreaterOrEqual;
      else if (next_token == "==")
         return ast::Binary_Operator::Equal;
      else if (next_token == "!=")
         return ast::Binary_Operator::NotEqual;
      else if (next_token == "&&")
         return ast::Binary_Operator::And;
      else if (next_token == "||")
         return ast::Binary_Operator::Or;
      else if (next_token == "|")
         return ast::Binary_Operator::BitOr;
      else if (next_token == "&")
         return ast::Binary_Operator::BitAnd;
      else if (next_token == "^")
         return ast::Binary_Operator::BitXor;
      else if (next_token == ">>")
         return ast::Binary_Operator::BitRightShift;
      else if (next_token == "<<")
         return ast::Binary_Operator::BitLeftShift;
      return ast::Binary_Operator::Invalid;
   }

   // Parses only compound operators for right association
   ast::Compound_Operator parse_comop(std::list<std::string> &tokens)
   {
      std::string next_token(tokens.front());
      tokens.pop_front();
      if (next_token == "+=")
         return ast::Compound_Operator::AdditionAssignment;
      else if (next_token == "-=")
         return ast::Compound_Operator::SubtractionAssignment;
      else if (next_token == "*=")
         return ast::Compound_Operator::MultiplicationAssignment;
      else if (next_token == "/=")
         return ast::Compound_Operator::DivisionAssignment;
      else if (next_token == "%=")
         return ast::Compound_Operator::ModulusAssignment;
      else if (next_token == "&=")
         return ast::Compound_Operator::BitwiseAndAssignment;
      else if (next_token == "|=")
         return ast::Compound_Operator::BitwiseOrAssignment;
      else if (next_token == "^=")
         return ast::Compound_Operator::BitwiseXorAssignment;
      else if (next_token == ">>=")
         return ast::Compound_Operator::RightShiftAssignment;
      else if (next_token == "<<=")
         return ast::Compound_Operator::LeftShiftAssignment;
      return ast::Compound_Operator::Invalid;
   }

   // This function hard codes each symbol's precedence values
   uint16_t precedence(const std::string &next_token)
   {
      if (next_token == "*" || next_token == "/" || next_token == "%")
         return 50;
      else if (next_token == "+" || next_token == "-")
         return 45;
      else if (next_token == "<<" || next_token == ">>")
         return 40;
      else if (next_token == "<" || next_token == "<=" || next_token == ">" || next_token == ">=")
         return 35;
      else if (next_token == "==" || next_token == "!=")
         return 30;
      else if (next_token == "&")
         return 25;
      else if (next_token == "^")
         return 20;
      else if (next_token == "|")
         return 15;
      else if (next_token == "&&")
         return 10;
      else if (next_token == "||")
         return 5;
      else if (next_token == "?")
         return 3;
      else if (next_token == "=" || (std::find(compound_operators.begin(), compound_operators.end(), next_token) != compound_operators.end()))
         return 1;
      return 0;
   }

   ast::Expression *parse_conditional_middle(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      expect("?", tokens);
      ast::Expression *middle = parse_expression(tokens, 0, expressions);
      expect(":", tokens);
      return middle;
   }


   // This function groups up expressions and orders them by precedence to create a recursive branch of Binary AST nodes
   ast::Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      ast::Expression *left = parse_factor(tokens, expressions);
      if (tokens.empty())
      {
         throw std::runtime_error("Expected ;, but found nothing.");
      }
      std::string next_token(tokens.front());
      ast::Expression *right;
      while (
          (std::find(binary_operators.begin(), binary_operators.end(), next_token) != binary_operators.end() || std::find(compound_operators.begin(), compound_operators.end(), next_token) != compound_operators.end()) &&
          precedence(next_token) >= min_prec)
      { // Ensures that we process preceeding operators first
         if (next_token == "=")
         {
            expect("=", tokens);
            right = parse_expression(tokens, precedence(next_token), expressions);
            std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Assignment>(left, right);
            expressions.push_back(std::move(unique_left));
         }
         else if (std::find(compound_operators.begin(), compound_operators.end(), next_token) != compound_operators.end())
         {
            ast::Compound_Operator compound_operator = parse_comop(tokens);
            right = parse_expression(tokens, precedence(next_token), expressions);
            std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Compound>(compound_operator, left, right);
            expressions.push_back(std::move(unique_left));
         }
         else if (next_token == "?")
         {
            ast::Expression *middle = parse_conditional_middle(tokens, expressions);
            right = parse_expression(tokens, precedence(next_token), expressions);
            std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Conditional>(left, middle, right);
            expressions.push_back(std::move(unique_left));
         }
         else
         {
            ast::Binary_Operator binary_operator = parse_binop(tokens);
            right = parse_expression(tokens, precedence(next_token) + 1, expressions); // The +1 ensures we are grouping from the left side to the right side
            std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Binary>(binary_operator, left, right);
            expressions.push_back(std::move(unique_left));
         }
         left = expressions.back().get();
         next_token = tokens.front();
      }

      return left;
   }

   // This recursive function Establishes Expression Nodes
   ast::Expression *parse_factor(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      if (tokens.empty())
      {
         throw std::runtime_error("Expected expression after operator, but found nothing.");
      }
      std::string next_token(tokens.front());
      char *endptr;
      long value = std::strtol(next_token.c_str(), &endptr, 10); // parses decimal integers
      if (*endptr == '\0') // valid integer
      {
         std::unique_ptr<ast::Constant> constant = std::make_unique<ast::Constant>(value);
         expressions.push_back(std::move(constant));
         tokens.pop_front();
         return expressions.back().get();
      }
      else if (std::find(unary_operators.begin(), unary_operators.end(), next_token) != unary_operators.end())
      {
         ast::Unary_Operator unary_operator = parse_unop(tokens);
         ast::Expression *inner_exp = parse_factor(tokens, expressions); // A Unary Expression Can contain another Unary Expression Whitin
         std::unique_ptr<ast::Unary> unop = std::make_unique<ast::Unary>(unary_operator, inner_exp);
         expressions.push_back(std::move(unop));
         return expressions.back().get();
      }
      else if (next_token == "(")
      { // Separating Unary Operators
         tokens.pop_front();
         ast::Expression *inner_exp = parse_expression(tokens, 0, expressions);
         expect(")", tokens);
         return inner_exp;
      }
      else if ( // find std method that combines these
          std::find(binary_operators.begin(), binary_operators.end(), next_token) == binary_operators.end() &&
          std::find(compound_operators.begin(), compound_operators.end(), next_token) == compound_operators.end() &&
          std::find(unary_operators.begin(), unary_operators.end(), next_token) == unary_operators.end())
      {
         ast::Identifier *name = new ast::Identifier(next_token);
         tokens.pop_front();
         if(tokens.front() == "(") {
            tokens.pop_front();
            std::vector<ast::Expression *> arguemnt_list;
            while(!tokens.empty() && tokens.front() != ")") {
               ast::Expression *arguement = parse_expression(tokens, 0, expressions);
               arguemnt_list.push_back(std::move(arguement));
               if(tokens.front() == ")") break;
               else {
                  expect(",", tokens);
                  if(!tokens.empty() && tokens.front() == ")") {
                     throw std::runtime_error(std::format("Trailing comma in argument list of function call {}.", name->text));
                  }
               }
            }
            expect(")", tokens);
            std::unique_ptr<ast::Function_Call> function_call = std::make_unique<ast::Function_Call>(name, arguemnt_list); // function call doesn't own expressions so copy by value is fine
            expressions.push_back(std::move(function_call));
         } else {
            std::unique_ptr<ast::Var> variable = std::make_unique<ast::Var>(name);
            expressions.push_back(std::move(variable));
         }
         return expressions.back().get();
      }
      else
      {
         throw std::runtime_error(std::format("Malformed Expression: {}", tokens.front()));
      }
   }

   // This function handles statements
   ast::Statement *parse_statement(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      if (tokens.front() == "return")
      {
         expect("return", tokens);
         ast::Expression *return_val = parse_expression(tokens, 0, expressions);
         expect(";", tokens);
         ast::Return *ret = new ast::Return(return_val);
         return ret;
      }
      else if (tokens.front() == "if")
      {
         tokens.pop_front();
         expect("(", tokens);
         ast::Expression *condition = parse_expression(tokens, 0, expressions);
         expect(")", tokens);
         ast::Statement *body = parse_statement(tokens, expressions);
         ast::If *if_statement;
         if (tokens.front() == "else")
         {
            tokens.pop_front();
            ast::Statement *else_block = parse_statement(tokens, expressions);
            if_statement = new ast::If(condition, body, else_block);
         }
         else
         {
            if_statement = new ast::If(condition, body);
         }
         return if_statement;
      }
      else if (tokens.front() == "break") {
         tokens.pop_front();
         ast::Identifier *label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
         ast::Break *break_statement = new ast::Break(label);
         expect(";", tokens);
         return break_statement;
      }
      else if (tokens.front() == "continue") {
         tokens.pop_front();
         ast::Identifier *label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
         ast::Continue *continue_statement = new ast::Continue(label);
         expect(";", tokens);
         return continue_statement;
      }
      else if (tokens.front() == "while") {
         tokens.pop_front();
         expect("(", tokens);
         ast::Expression *condition = parse_expression(tokens, 0, expressions);
         expect(")", tokens);
         ast::Statement *body = parse_statement(tokens, expressions);
         ast::Identifier *label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
         ast::While *while_statement = new ast::While(condition, body, label);
         return while_statement;
      }
      else if (tokens.front() == "do") {
         tokens.pop_front();
         ast::Statement *body = parse_statement(tokens, expressions);
         expect("while", tokens);
         expect("(", tokens);
         ast::Expression *condition = parse_expression(tokens, 0, expressions);
         expect(")", tokens);
         expect(";", tokens);
         ast::Identifier *label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
         ast::DoWhile *do_while_statement = new ast::DoWhile(body, condition, label);
         return do_while_statement;
      }
      else if (tokens.front() == "for") {
         tokens.pop_front();
         expect("(", tokens);
         ast::For_Init *init;
         if (type_aliases.count(tokens.front()) || std::find(specifiers.begin(), specifiers.end(), tokens.front()) != specifiers.end()) {
            std::pair<ast::Type *, ast::Storage_Class> type_and_storage_class = parse_type_and_storage_class(tokens);
            ast::Var_Decl *variable_declaration = dynamic_cast<ast::Var_Decl *>(parse_declaration(tokens, expressions, type_and_storage_class));
            if(!variable_declaration) {
               throw std::runtime_error("Expected variable declaration in for loop.");
            }
            init = new ast::Init_Decl(variable_declaration);
         } else {
            if(tokens.front() == ";") {
               tokens.pop_front();
               init = new ast::Init_Exp();
            } else {
               ast::Expression *exp = parse_expression(tokens, 0, expressions);
               init = new ast::Init_Exp(exp);
               expect(";", tokens);
            }
         }

         ast::Expression *condition = nullptr;
         if(tokens.front() != ";") {
            condition = parse_expression(tokens, 0, expressions);
         } 
         expect(";", tokens);

         ast::Expression *post = nullptr;
         if(tokens.front() != ")") {
            post = parse_expression(tokens, 0, expressions);
         } 
         expect(")", tokens);
         ast::Statement *body = parse_statement(tokens, expressions);
         ast::Identifier *label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
         ast::For *for_statement = new ast::For(init, body, label, condition, post);
         return for_statement;
      }
      else if (tokens.front() == ";")
      {
         expect(";", tokens);
         ast::Null *null_statement = new ast::Null();
         return null_statement;
      }
      else if (tokens.front() == "{") 
      {
         expect("{", tokens);
         std::vector<std::unique_ptr<ast::Block_Item>> block_items;
         while(tokens.front() != "}") {
            block_items.push_back(parse_block_item(tokens, expressions));
         }
         ast::Block *block = new ast::Block(std::move(block_items));
         expect("}", tokens);
         ast::Compound_Statement *compound = new ast::Compound_Statement(block);
         return compound;
      }
      else
      {
         ast::Expression *exp = parse_expression(tokens, 0, expressions);
         ast::Expression_Statement *exp_statement = new ast::Expression_Statement(exp);
         expect(";", tokens);
         return exp_statement;
      }
      return nullptr;
   }

   ast::Declaration *parse_declaration(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions, std::pair<ast::Type *, ast::Storage_Class> &type_and_storage_class) {
      ast::Identifier *identifier;
      ast::Declaration *declaration;
      char *endptr;
      std::strtol(tokens.front().c_str(), &endptr, 10); // parses decimal integers
      if (std::next(tokens.begin()) == tokens.end())
      {
         throw std::runtime_error("Missing semicolon.");
      }
      std::string next_token(*std::next(tokens.begin()));
      if (*endptr != '\0' && tokens.front() != "return" && std::regex_match(tokens.front(), parser::naming_convention))
      {
         identifier = new ast::Identifier(tokens.front());
         tokens.pop_front();
      }
      else
      {
         throw std::runtime_error(std::format("{} is not a valid variable or function name.", next_token));
      }
      if (next_token == "=")
      {
         expect("=", tokens);
         ast::Expression *exp = parse_expression(tokens, 0, expressions);
         declaration = new ast::Var_Decl(identifier, exp, type_and_storage_class.second);
         expect(";", tokens);
      }
      else if (next_token == ";")
      {
         declaration = new ast::Var_Decl(identifier, nullptr, type_and_storage_class.second);
         expect(";", tokens);
      }
      else if (next_token == "(") // resolve_function will handle whether or not this is allowed to contain a definition
      {
         expect("(", tokens);
         std::vector<std::unique_ptr<ast::Identifier>> param_list;
         parse_parameters(tokens, param_list, identifier->text);
         ast::Block *body = nullptr;
         if(tokens.front() == "{") // Function declaration with a definition
         {
            tokens.pop_front();
            std::vector<std::unique_ptr<ast::Block_Item>> function_body;
            while (tokens.front() != "}")
            {
               std::unique_ptr<ast::Block_Item> next_block_item = parse_block_item(tokens, expressions);
               function_body.push_back(std::move(next_block_item));
            }
            expect("}", tokens);
            body = new ast::Block(std::move(function_body));
         }
         else // Basic function declaration without a definition
         {
            expect(";", tokens);
         }
         declaration = new ast::Fun_Decl(identifier, std::move(param_list), std::move(expressions), body, type_and_storage_class.second);
      }
      else
      {
         throw std::runtime_error("Not a valid declaration.");
      }
      return declaration;
   }

   std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      // handling typedef
      if (tokens.front() == "typedef")
      {
         tokens.pop_front();
         if (!type_aliases.count(tokens.front()))
         {
            throw std::runtime_error(std::format("{} is not a valid type.", tokens.front()));
         }

         std::string data_type(tokens.front());
         if (type_aliases.count(data_type))
         { // handle nested typedef
            data_type = type_aliases[data_type];
         }
         tokens.pop_front();

         std::string alias(tokens.front());
         tokens.pop_front();
         type_aliases[alias] = data_type;

         std::unique_ptr<ast::S> s = std::make_unique<ast::S>(new ast::Null());
         return s;
      }
      if (type_aliases.count(tokens.front()) || std::find(specifiers.begin(), specifiers.end(), tokens.front()) != specifiers.end())
      {
         std::pair<ast::Type *, ast::Storage_Class> type_and_storage_class = parse_type_and_storage_class(tokens);
         std::unique_ptr<ast::D> d = std::make_unique<ast::D>(parse_declaration(tokens, expressions, type_and_storage_class));
         // delete declaration;
         return d;
      }
      else
      {
         ast::Statement *unresolved_statement = parse_statement(tokens, expressions);
         std::unique_ptr<ast::S> s = std::make_unique<ast::S>(unresolved_statement);
         return s;
      }
   }

   void resolve_exp(ast::Expression *e, std::unordered_map<std::string, MapEntry> &identifier_map)
   {
      ast::Assignment *assignment = dynamic_cast<ast::Assignment *>(e);
      if (assignment)
      {
         ast::Var *var_node = dynamic_cast<ast::Var *>(assignment->lvalue);
         if (!var_node)
         {
            throw std::runtime_error("Invalid lvalue.");
         }
         resolve_exp(assignment->lvalue, identifier_map);
         resolve_exp(assignment->exp, identifier_map);
         return;
      }
      ast::Compound *compound = dynamic_cast<ast::Compound *>(e);
      if (compound)
      {
         ast::Var *var_node = dynamic_cast<ast::Var *>(compound->left);
         if (!var_node)
         {
            throw std::runtime_error("Invalid lvalue.");
         }
         resolve_exp(compound->left, identifier_map);
         resolve_exp(compound->right, identifier_map);
         return;
      }
      ast::Binary *binary = dynamic_cast<ast::Binary *>(e);
      if (binary)
      {
         resolve_exp(binary->left, identifier_map);
         resolve_exp(binary->right, identifier_map);
         return;
      }
      ast::Unary *unary = dynamic_cast<ast::Unary *>(e);
      if (unary)
      {
         resolve_exp(unary->exp, identifier_map);
         return;
      }
      ast::Var *var = dynamic_cast<ast::Var *>(e);
      if (var)
      {
         if (identifier_map.count(var->identifier->text))
         {
            var->identifier->text = identifier_map[var->identifier->text].new_name;
            return;
         }
         else
         {
            throw std::runtime_error(std::format("{} is an undeclared variable.", var->identifier->text));
         }
      }
      ast::Conditional *conditional = dynamic_cast<ast::Conditional *>(e);
      if (conditional)
      {
         resolve_exp(conditional->condition, identifier_map);
         resolve_exp(conditional->left, identifier_map);
         resolve_exp(conditional->right, identifier_map);
         return;
      }
      ast::Function_Call *function_call = dynamic_cast<ast::Function_Call *>(e);
      if(function_call) {
         if(identifier_map.count(function_call->identifier->text)) 
         {
            function_call->identifier->text = identifier_map[function_call->identifier->text].new_name;
            for(auto &arg: function_call->args) {
               resolve_exp(arg, identifier_map);
            }
            return;
         }
         else 
         {
            throw std::runtime_error(std::format("{} is an undeclared function.", function_call->identifier->text));
         }
      }
   }

   void resolve_block(ast::Block *block, std::unordered_map<std::string, MapEntry> &identifier_map, bool is_file_scope){
      for (auto &item: block->block_items){
         ast::D *d = dynamic_cast<ast::D *>(item.get());
         ast::S *s = dynamic_cast<ast::S *>(item.get());

         if (d){
            resolve_declaration(d->declaration, identifier_map, is_file_scope);
         }
         else if (s){
            resolve_statement(s->statement, identifier_map);
         }
      }
   }

   void resolve_declaration(ast::Declaration *declaration, std::unordered_map<std::string, MapEntry> &identifier_map, bool is_file_scope)
   {
      ast::Var_Decl *var_decl = dynamic_cast<ast::Var_Decl *>(declaration);
      ast::Fun_Decl *fun_decl = dynamic_cast<ast::Fun_Decl *>(declaration);
      if(var_decl) {
         resolve_var_decl(var_decl, identifier_map);
      }
      if(fun_decl) {
         resolve_fun_decl(fun_decl, identifier_map, is_file_scope);
      }
   }

   void resolve_var_decl(ast::Var_Decl *var_decl, std::unordered_map<std::string, MapEntry> &identifier_map) {
      if(!std::regex_match(var_decl->name->text, parser::naming_convention)) {
         throw std::runtime_error(std::format("{} is not a valid name for a variable.", var_decl->name->text));
      }
      if (identifier_map.count(var_decl->name->text) && identifier_map[var_decl->name->text].from_current_scope)
      {
         throw std::runtime_error(std::format("{} has already been declared.", var_decl->name->text));
      }
      std::string unique_name = make_temporary(var_decl->name->text);
      identifier_map.insert_or_assign(var_decl->name->text, MapEntry{ unique_name, true, false });
      var_decl->name->text = unique_name;
      if (var_decl->init)
      {
         resolve_exp(var_decl->init, identifier_map);
      }
   }

   void resolve_fun_decl(ast::Fun_Decl *fun_decl, std::unordered_map<std::string, MapEntry> &identifier_map, bool is_file_scope) {
      std::string function_name = fun_decl->name->text;
      if(!std::regex_match(function_name, parser::naming_convention)) {
         throw std::runtime_error(std::format("{} is not a valid name for a function.", function_name));
      }
      if(identifier_map.count(function_name))
      {
         MapEntry& prev_entry = identifier_map[function_name];
         if(prev_entry.from_current_scope && !prev_entry.has_linkage) {
            throw std::runtime_error(std::format("The function {} has already been declared.", function_name));
         }
      }

      identifier_map.insert_or_assign(function_name, MapEntry{ function_name, true, true });

      std::unordered_map<std::string, MapEntry> inner_map = copy_identifier_map(identifier_map);
      for (auto& param : fun_decl->params)
      {
         resolve_params(param.get(), inner_map);
      }

      if(fun_decl->body) {
         if(!is_file_scope) {
            throw std::runtime_error("Function definitions are not supported in this scope.");
         }
         resolve_block(fun_decl->body, inner_map, false);
         fun_decl->body = label_block(fun_decl->body, nullptr); // labels own block after resolving it for a fully complete ast node
      }
   }

   void resolve_params(ast::Identifier *identifier, std::unordered_map<std::string, MapEntry> &identifier_map) {
      if(!std::regex_match(identifier->text, parser::naming_convention)) {
         throw std::runtime_error(std::format("{} is not a valid name for a parameter.", identifier->text));
      }
      if (identifier_map.count(identifier->text) && identifier_map[identifier->text].from_current_scope)
      {
         throw std::runtime_error(std::format("The parameter {} has already been declared.", identifier->text));
      }

      std::string unique_name = make_temporary(identifier->text);
      identifier_map.insert_or_assign(identifier->text, MapEntry{ unique_name, true, false });
      identifier->text = unique_name;
   }

   void resolve_statement(ast::Statement *statement, std::unordered_map<std::string, MapEntry> &identifier_map)
   {
      ast::Return *ret = dynamic_cast<ast::Return *>(statement);
      if (ret)
      {
         resolve_exp(ret->exp, identifier_map);
         return;
      }
      ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(statement);
      if (exp_statement)
      {
         resolve_exp(exp_statement->exp, identifier_map);
         return;
      }
      ast::If *if_statement = dynamic_cast<ast::If *>(statement);
      if (if_statement)
      {
         resolve_exp(if_statement->condition, identifier_map);
         resolve_statement(if_statement->then_statement, identifier_map);
         if (if_statement->else_statement)
         {
            resolve_statement(if_statement->else_statement, identifier_map);
         }
         return;
      }
      ast::Compound_Statement *compound_statement = dynamic_cast<ast::Compound_Statement *>(statement);
      if (compound_statement){
         std::unordered_map<std::string, MapEntry> new_identifier_map = copy_identifier_map(identifier_map);
         resolve_block(compound_statement->block, new_identifier_map, false);
         return;
      }
      ast::Break *break_statement = dynamic_cast<ast::Break *>(statement);
      if (break_statement) {
         delete break_statement->label;
         break_statement->label = make_label(break_statement->label->text);
         return;
      }
      ast::Continue *continue_statement = dynamic_cast<ast::Continue *>(statement);
      if (continue_statement) {
         delete continue_statement->label;
         continue_statement->label = make_label(continue_statement->label->text);
         return;
      }
      ast::For *for_statement = dynamic_cast<ast::For *>(statement);
      if (for_statement) {
         std::unordered_map<std::string, MapEntry> new_identifier_map = copy_identifier_map(identifier_map);
         resolve_for_init(for_statement->init, new_identifier_map);
         resolve_optional_exp(for_statement->condition, new_identifier_map);
         resolve_optional_exp(for_statement->post, new_identifier_map);
         resolve_statement(for_statement->body, new_identifier_map);
         delete for_statement->label;
         for_statement->label = make_label(for_statement->label->text);
         return;
      }
      ast::While *while_statement = dynamic_cast<ast::While *>(statement);
      if (while_statement) {
         resolve_exp(while_statement->condition, identifier_map);
         resolve_statement(while_statement->body, identifier_map);
         delete while_statement->label;
         while_statement->label = make_label(while_statement->label->text);
         return;
      }
      ast::DoWhile *do_while_statement = dynamic_cast<ast::DoWhile *>(statement);
      if (do_while_statement) {
         resolve_statement(do_while_statement->body, identifier_map);
         resolve_exp(do_while_statement->condition, identifier_map);
         delete do_while_statement->label;
         do_while_statement->label = make_label(do_while_statement->label->text);
         return;
      }
   }

   void resolve_for_init(ast::For_Init *init, std::unordered_map<std::string, MapEntry> &identifier_map) {
      ast::Init_Decl *init_decl = dynamic_cast<ast::Init_Decl *>(init);
      ast::Init_Exp *init_exp = dynamic_cast<ast::Init_Exp *>(init);
      if(init_decl) {
         resolve_declaration(init_decl->variable_declaration, identifier_map, false);
      } else if (init_exp) {
         resolve_optional_exp(init_exp->expression, identifier_map);
      }
   }

   void resolve_optional_exp(ast::Expression *exp, std::unordered_map<std::string, MapEntry> &identifier_map) {
      if (exp) {
         resolve_exp(exp, identifier_map);
      }
   }

   ast::Statement *annotate(ast::Statement *statement, ast::Identifier *current_label) {
      ast::Break *break_statement = dynamic_cast<ast::Break *>(statement);
      if(break_statement) {
         delete break_statement->label;
         break_statement->label = current_label;
         return break_statement;
      }
      ast::Continue *continue_statement = dynamic_cast<ast::Continue *>(statement);
      if (continue_statement) {
         delete continue_statement->label;
         continue_statement->label = current_label;
         return continue_statement;
      }
      ast::While *while_statement = dynamic_cast<ast::While *>(statement);
      if (while_statement) {
         delete while_statement->label;
         while_statement->label = current_label;
         return while_statement;
      }
      ast::DoWhile *do_while_statement = dynamic_cast<ast::DoWhile *>(statement);
      if (do_while_statement) {
         delete do_while_statement->label;
         do_while_statement->label = current_label;
         return do_while_statement;
      }
      ast::For *for_statement = dynamic_cast<ast::For *>(statement);
      if (for_statement) {
         delete for_statement->label;
         for_statement->label = current_label;
         return for_statement;
      }
      return statement;
   }

   // ugly, but faster returns
   ast::Statement *label_statement(ast::Statement *statement, ast::Identifier *current_label) {
      ast::Break *break_statement = dynamic_cast<ast::Break *>(statement);
      if(break_statement) {
         if(!current_label) {
            throw std::runtime_error("Break statement outside of loop.");
         }
         ast::Identifier *copy = make_label(current_label->text);
         return annotate(break_statement, copy); 
      }
      ast::Continue *continue_statement = dynamic_cast<ast::Continue *>(statement);
      if (continue_statement) {
         if(!current_label) {
            throw std::runtime_error("Continue statement outside of loop.");
         }
         ast::Identifier *copy = make_label(current_label->text);
         return annotate(continue_statement, copy); 
      }
      ast::Compound_Statement *compound_statement = dynamic_cast<ast::Compound_Statement *>(statement);
      if (compound_statement) {
         compound_statement->block = label_block(compound_statement->block, current_label);
         return compound_statement;
      }
      ast::If *if_statement = dynamic_cast<ast::If *>(statement);
      if (if_statement) {
         if_statement->then_statement = label_statement(if_statement->then_statement, current_label);
         if (if_statement->else_statement) {
            if_statement->else_statement = label_statement(if_statement->else_statement, current_label);
         }
         return if_statement;
      }
      ast::Expression_Statement *expression_statement = dynamic_cast<ast::Expression_Statement *>(statement);
      ast::Return *return_statement = dynamic_cast<ast::Return *>(statement);
      ast::Null *null = dynamic_cast<ast::Null *>(statement);
      if(expression_statement || return_statement || null) {
         return statement;
      }
      ast::While *while_statement = dynamic_cast<ast::While *>(statement);
      ast::DoWhile *do_while_statement = dynamic_cast<ast::DoWhile *>(statement);
      ast::For *for_statement = dynamic_cast<ast::For *>(statement);
      if (while_statement || do_while_statement || for_statement) {
         ast::Identifier *new_label = make_label();
         if(while_statement) {
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

   ast::Block *label_block(ast::Block *block, ast::Identifier *current_label) {
      for (auto &item: block->block_items) {
         ast::S *s = dynamic_cast<ast::S *>(item.get());
         if(s) {
            s->statement = label_statement(s->statement, current_label);
         }
      }
      return block;
   }

   void typecheck_declaration(ast::Declaration *declaration, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, bool>> &symbols) {
      ast::Var_Decl *var_decl = dynamic_cast<ast::Var_Decl *>(declaration);
      ast::Fun_Decl *fun_decl = dynamic_cast<ast::Fun_Decl *>(declaration);
      if(var_decl) {
         typecheck_variable_declaration(var_decl, symbols);
      }
      else if(fun_decl) {
         typecheck_function_declaration(fun_decl, symbols);
      }
      else {
         throw std::runtime_error("Invalid declaration.");
      }
   }

   void typecheck_variable_declaration(ast::Var_Decl *var_decl, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, bool>> &symbols) {
      if(var_decl->name->text != "void") {
         symbols.emplace(var_decl->name->text, std::make_pair(std::make_unique<ast::Int>(), true)); // will not overwrite if already exists
      }
      if(var_decl->init) {
         typecheck_exp(var_decl->init, symbols);
      }
   }

   void typecheck_function_declaration(ast::Fun_Decl *fun_decl, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, bool>> &symbols) {
      std::unique_ptr<ast::Fun_Type> fun_type = std::make_unique<ast::Fun_Type>(fun_decl->params.size());
      ast::Block *body = fun_decl->body;
      bool already_defined = false;

      if(symbols.count(fun_decl->name->text)) {
         std::pair<std::unique_ptr<ast::Type>, bool> &old_decl = symbols[fun_decl->name->text];
         ast::Fun_Type *old_fun_type = dynamic_cast<ast::Fun_Type *>(old_decl.first.get());
         if(!old_fun_type || old_fun_type->param_count != fun_type->param_count) {
            throw std::runtime_error("Incompatible function declarations.");
         }
         already_defined = old_decl.second;
         if(already_defined && body) {
            throw std::runtime_error("Function is defined more than once.");
         } 
      }

      symbols.insert_or_assign(fun_decl->name->text, std::make_pair(std::move(fun_type), already_defined || (body != nullptr)));
      if (body) {
         for(auto &param : fun_decl->params) {
            symbols.emplace(param->text, std::make_pair(std::make_unique<ast::Int>(), false));
         }
         typecheck_block(body, symbols);
      }
   }

   // add type checking for statements later
   void typecheck_block(ast::Block *block, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, bool>> &symbols) {
      for(auto &item : block->block_items) {
         ast::D *d = dynamic_cast<ast::D *>(item.get());
         if(d) {
            if(dynamic_cast<ast::Var_Decl *>(d->declaration)) {
               typecheck_variable_declaration(dynamic_cast<ast::Var_Decl *>(d->declaration), symbols);
            }
            else if(dynamic_cast<ast::Fun_Decl *>(d->declaration)) {
               typecheck_function_declaration(dynamic_cast<ast::Fun_Decl *>(d->declaration), symbols);
            }
         }
         ast::S *s = dynamic_cast<ast::S *>(item.get());
         if(s) {
            typecheck_statement(s->statement, symbols);
         }
      }
   }

   // add type checking for every other expression type later
   void typecheck_exp(ast::Expression *e, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, bool>> &symbols) {
      ast::Function_Call *function_call = dynamic_cast<ast::Function_Call *>(e);
      if(function_call) {
         auto it = symbols.find(function_call->identifier->text);
         if(it == symbols.end()) {
            throw std::runtime_error(std::format("Function {} not defined or declared in scope.", function_call->identifier->text));
         }
         ast::Fun_Type *f_type = dynamic_cast<ast::Fun_Type *>(it->second.first.get());
         if(!f_type) {
            throw std::runtime_error(std::format("Variable {} used as function name.", function_call->identifier->text));
         }
         if(f_type->param_count != function_call->args.size()) {
            throw std::runtime_error(std::format("Function {} takes {} arguments, but {} were provided.", function_call->identifier->text, f_type->param_count, function_call->args.size()));
         }
         for(auto &arg: function_call->args) {
            typecheck_exp(arg, symbols);
         }
         return;
      }
      ast::Var *var = dynamic_cast<ast::Var *>(e);
      if(var) {
         std::pair<std::unique_ptr<ast::Type>, bool> &var_type = symbols[var->identifier->text];
         ast::Int *int_type = dynamic_cast<ast::Int *>(var_type.first.get());
         if(!int_type) {
            throw std::runtime_error(std::format("Function name {} used as variable.", var->identifier->text));
         }
         return;
      }
      ast::Assignment *assignment = dynamic_cast<ast::Assignment *>(e);
      if(assignment) {
         typecheck_exp(assignment->lvalue, symbols);
         typecheck_exp(assignment->exp, symbols);
         return;
      }
      ast::Binary *binary = dynamic_cast<ast::Binary *>(e);
      if(binary) {
         typecheck_exp(binary->left, symbols);
         typecheck_exp(binary->right, symbols);
         return;
      }
      ast::Compound *compound = dynamic_cast<ast::Compound *>(e);
      if(compound) {
         typecheck_exp(compound->left, symbols);
         typecheck_exp(compound->right, symbols);
         return;
      }
      ast::Conditional *conditional = dynamic_cast<ast::Conditional *>(e);
      if(conditional) {
         typecheck_exp(conditional->condition, symbols);
         typecheck_exp(conditional->left, symbols);
         typecheck_exp(conditional->right, symbols);
         return;
      }
      ast::Unary *unary = dynamic_cast<ast::Unary *>(e);
      if(unary) {
         typecheck_exp(unary->exp, symbols);
         return;
      }
   }
   
   void typecheck_statement(ast::Statement *statement, std::unordered_map<std::string, std::pair<std::unique_ptr<ast::Type>, bool>> &symbols) {
      ast::Expression_Statement *expression_statement = dynamic_cast<ast::Expression_Statement *>(statement);
      if(expression_statement) {
         typecheck_exp(expression_statement->exp, symbols);
         return;
      }
      ast::Return *return_statement = dynamic_cast<ast::Return *>(statement);
      if(return_statement) {
         typecheck_exp(return_statement->exp, symbols);
         return;
      }
      ast::If *if_statement = dynamic_cast<ast::If *>(statement);
      if(if_statement) {
         typecheck_exp(if_statement->condition, symbols);
         typecheck_statement(if_statement->then_statement, symbols);
         if(if_statement->else_statement) {
            typecheck_statement(if_statement->else_statement, symbols);
         }
         return;
      }
      ast::Compound_Statement *compound_statement = dynamic_cast<ast::Compound_Statement *>(statement);
      if(compound_statement) {
         typecheck_block(compound_statement->block, symbols);
         return;
      }
      ast::For *for_statement = dynamic_cast<ast::For *>(statement);
      if(for_statement) {
         ast::Init_Decl *init_decl = dynamic_cast<ast::Init_Decl *>(for_statement->init);
         if(init_decl) {
            typecheck_variable_declaration(dynamic_cast<ast::Var_Decl *>(init_decl->variable_declaration), symbols);
         }
         ast::Init_Exp *init_exp = dynamic_cast<ast::Init_Exp *>(for_statement->init);
         if(init_exp && init_exp->expression) {
            typecheck_exp(init_exp->expression, symbols);
         }
         if(for_statement->condition) {
            typecheck_exp(for_statement->condition, symbols);
         }
         if(for_statement->post) {
            typecheck_exp(for_statement->post, symbols);
         }
         typecheck_statement(for_statement->body, symbols);
         return;
      }
      ast::While *while_statement = dynamic_cast<ast::While *>(statement);
      if(while_statement) {
         typecheck_exp(while_statement->condition, symbols);
         typecheck_statement(while_statement->body, symbols);
         return;
      }
      ast::DoWhile *do_while_statement = dynamic_cast<ast::DoWhile *>(statement);
      if(do_while_statement) {
         typecheck_statement(do_while_statement->body, symbols);
         typecheck_exp(do_while_statement->condition, symbols);
         return;
      }
   }

   void parse_parameters(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Identifier>> &params, const std::string &func_name) {
      while(!tokens.empty() && tokens.front() != ")")
      {
         if(type_aliases.count(tokens.front()) || "int" == tokens.front())
         {
            tokens.pop_front();
            params.push_back(std::make_unique<ast::Identifier>(tokens.front()));
            tokens.pop_front();
            if(tokens.front() == ")") break;
            else {
               expect(",", tokens);
               if(!tokens.empty() && !(type_aliases.count(tokens.front()) || tokens.front() == "int")) {
                  throw std::runtime_error(std::format("Trailing comma in parameter list of function {}.", func_name));
               }
            }
         } 
         else if(tokens.front() == "void")
         {
            tokens.pop_front();
            break;
         }
         else
         {
            throw std::runtime_error(std::format("{} should not be in the parameter list of function {}.", tokens.front(), func_name));
         }
      }
      expect(")", tokens);
   }

   // this function is a bit different from the textbook, but it allows us to handle the entire parsing process for specifiers in one function
   std::pair<ast::Type *, ast::Storage_Class> parse_type_and_storage_class(std::list<std::string> &tokens) {
      std::vector<std::string> specifier_list;
      while(!tokens.empty() && (type_aliases.count(tokens.front()) || std::find(specifiers.begin(), specifiers.end(), tokens.front()) != specifiers.end()))
      {
         specifier_list.push_back(tokens.front());
         tokens.pop_front();
      }
      if(tokens.empty()) throw std::runtime_error("Expected identifier after type specifiers, got end of file.");

      std::vector<std::string> types;
      std::vector<std::string> storage_classes;
      for(const std::string &specifier : specifier_list)
      {
         if(type_aliases.count(specifier) || specifier == "int")
         {
            types.push_back(specifier);
         } else {
            storage_classes.push_back(specifier);
         }
      }

      if(types.size() != 1) throw std::runtime_error("Invalid Type Specifier");
      if(storage_classes.size() > 1) throw std::runtime_error("Invalid Storage Class");

      ast::Int *type = nullptr; // for now, this is just a placeholder
      ast::Storage_Class storage_class;

      if(storage_classes.size() == 1) {
         storage_class = ast::get_storage_class(storage_classes[0]);
      } else {
         storage_class = ast::Storage_Class::NONE;
      }

      return std::make_pair(type, storage_class); // nothing uses type for now so this is okay
   }

   // Handles file scope declarations
   ast::Program *parse_program(std::list<std::string> &tokens)
   {
      std::vector<std::unique_ptr<ast::Declaration>> declarations;
      std::vector<std::unique_ptr<ast::Expression>> global_expressions; // this is used to store variable declarations that are not part of a function declaration
      while(!tokens.empty())
      {
         if(!(type_aliases.count(tokens.front()) || std::find(specifiers.begin(), specifiers.end(), tokens.front()) != specifiers.end())) {
            throw std::runtime_error(std::format("Expected variable or function declaration, got {}.", tokens.front()));
         }
         std::pair<ast::Type *, ast::Storage_Class> type_and_storage_class = parse_type_and_storage_class(tokens);

         std::vector<std::unique_ptr<ast::Expression>> expressions; // doesnt matter if this is empty after move since global variables don't own expressions anyways
         ast::Declaration *declaration = parse_declaration(tokens, expressions, type_and_storage_class);

         if(dynamic_cast<ast::Var_Decl *>(declaration)) {
            for(auto &expression : expressions) {
               global_expressions.push_back(std::move(expression)); // move global expressions to the global_expressions vector
            }
         }

         resolve_declaration(declaration, identifier_map, true);
         typecheck_declaration(declaration, symbols); // type checking after resolution
         declarations.emplace_back(declaration);
      }
      if (!tokens.empty())
      {
         throw std::runtime_error("Extra Characters Found For Minimal Compiler.");
      }

      identifier_map.clear();
      type_aliases.clear();
      symbols.clear();


      return new ast::Program(std::move(declarations), std::move(global_expressions));
   }

   // This function converts Tokens into AST Program node
   ast::Program *parse(std::list<std::string> &tokens)
   {
      return parse_program(tokens);
   }
}
