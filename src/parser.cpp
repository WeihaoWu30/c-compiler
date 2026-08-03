#include "ast/ast.hpp"
#include "ast/for_inits/init_exp.hpp"
#include "ast/statements/for.hpp"
#include "compiler/parser.hpp"
#include <stdexcept>
#include <format>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <compiler/ir_gen.hpp>
#include <memory>
#include <utility>
#include <iterator>

// This File is meant to convert the tokens into Abstract Syntax Tree nodes

namespace parser
{
   // For Parsing Expressions
   std::unordered_map<std::string, std::pair<std::string, bool>> variable_map;
   std::unordered_map<std::string, std::string> symbol_table;
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

   bool is_type(const std::string &token)
   {
      return token == "int" || symbol_table.count(token);
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

   std::unordered_map<std::string, std::pair<std::string, bool>> copy_variable_map(std::unordered_map<std::string, std::pair<std::string, bool>> &variable_map){
      std::unordered_map<std::string, std::pair<std::string, bool>> duplicate(variable_map);
      for ( auto &[real_name, scope] : duplicate) {
         scope.second = false;
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
      if (*endptr == '\0')                                       // valid integer
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
      else if (
          std::find(binary_operators.begin(), binary_operators.end(), next_token) == binary_operators.end() &&
          std::find(compound_operators.begin(), compound_operators.end(), next_token) == compound_operators.end() &&
          tokens.front() != "int")
      {
         std::unique_ptr<ast::Var> variable = std::make_unique<ast::Var>(new ast::Identifier(next_token));
         expressions.push_back(std::move(variable));
         tokens.pop_front();
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
         if (is_type(tokens.front())) {
            tokens.pop_front();
            ast::Declaration *declaration = parse_declaration(tokens, expressions);
            init = new ast::Init_Decl(declaration);
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

   ast::Declaration *parse_declaration(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions) {
      ast::Identifier *variable_name;
      ast::Declaration *declaration;
      char *endptr;
      std::strtol(tokens.front().c_str(), &endptr, 10); // parses decimal integers
      if (std::next(tokens.begin()) == tokens.end())
      {
         throw std::runtime_error("Missing semicolon.");
      }
      std::string variable_declaration(*std::next(tokens.begin()));
      if (*endptr != '\0' && tokens.front() != "return")
      {
         std::string name(tokens.front());
         // tokens.pop_front();
         variable_name = new ast::Identifier(name);
      }
      else
      {
         throw std::runtime_error(std::format("{} is not a valid variable name.", variable_declaration));
      }

      if (variable_declaration == "=")
      {
         // expect("=", tokens);
         ast::Expression *exp = parse_expression(tokens, 0, expressions);
         declaration = new ast::Declaration(variable_name, exp);
         expect(";", tokens);
      }
      else if (variable_declaration == ";")
      {
         tokens.pop_front();
         declaration = new ast::Declaration(variable_name);
         expect(";", tokens);
      }
      else
      {
         throw std::runtime_error("Not a valid block item.");
      }
      return declaration;
   }

   std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      // handling typedef
      if (tokens.front() == "typedef")
      {
         tokens.pop_front();
         if (!is_type(tokens.front()))
         {
            throw std::runtime_error(std::format("{} is not a valid type.", tokens.front()));
         }

         std::string data_type(tokens.front());
         if (symbol_table.count(data_type))
         { // handle nested typedef
            data_type = symbol_table[data_type];
         }
         tokens.pop_front();

         std::string alias(tokens.front());
         tokens.pop_front();
         symbol_table[alias] = data_type;

         std::unique_ptr<ast::S> s = std::make_unique<ast::S>(new ast::Null());
         return s;
      }
      if (tokens.front() == "int")
      {
         tokens.pop_front();
         std::unique_ptr<ast::D> d = std::make_unique<ast::D>(parse_declaration(tokens, expressions));
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

   ast::Expression *resolve_exp(ast::Expression *e, std::unordered_map<std::string, std::pair<std::string, bool>> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      ast::Assignment *assignment = dynamic_cast<ast::Assignment *>(e);
      if (assignment)
      {
         ast::Var *var_node = dynamic_cast<ast::Var *>(assignment->lvalue);
         if (!var_node)
         {
            throw std::runtime_error("Invalid lvalue.");
         }
         ast::Expression *lvalue = resolve_exp(assignment->lvalue, variable_map, expressions);
         ast::Expression *exp = resolve_exp(assignment->exp, variable_map, expressions);
         std::unique_ptr<ast::Assignment> a = std::make_unique<ast::Assignment>(lvalue, exp);
         expressions.push_back(std::move(a));
         return expressions.back().get();
      }
      ast::Compound *compound = dynamic_cast<ast::Compound *>(e);
      if (compound)
      {
         ast::Var *var_node = dynamic_cast<ast::Var *>(compound->left);
         if (!var_node)
         {
            throw std::runtime_error("Invalid lvalue.");
         }
         ast::Expression *left = resolve_exp(compound->left, variable_map, expressions);
         ast::Expression *right = resolve_exp(compound->right, variable_map, expressions);
         std::unique_ptr<ast::Compound> c = std::make_unique<ast::Compound>(compound->compound_operator, left, right);
         expressions.push_back(std::move(c));
         return expressions.back().get();
      }
      ast::Binary *binary = dynamic_cast<ast::Binary *>(e);
      if (binary)
      {
         ast::Expression *left = resolve_exp(binary->left, variable_map, expressions);
         ast::Expression *right = resolve_exp(binary->right, variable_map, expressions);
         std::unique_ptr<ast::Binary> b = std::make_unique<ast::Binary>(binary->binary_operator, left, right);
         expressions.push_back(std::move(b));
         return expressions.back().get();
      }
      ast::Unary *unary = dynamic_cast<ast::Unary *>(e);\
      if (unary)
      {
         ast::Expression *exp = resolve_exp(unary->exp, variable_map, expressions);
         std::unique_ptr<ast::Unary> u = std::make_unique<ast::Unary>(unary->unary_operator, exp);
         expressions.push_back(std::move(u));
         return expressions.back().get();
      }
      ast::Var *var = dynamic_cast<ast::Var *>(e);
      if (var)
      {
         if (variable_map.count(var->identifier->name))
         {
            std::unique_ptr<ast::Var> v = std::make_unique<ast::Var>(new ast::Identifier(variable_map[var->identifier->name].first));
            expressions.push_back(std::move(v));
            return expressions.back().get();
         }
         else
         {
            throw std::runtime_error(std::format("{} is an undeclared variable.", var->identifier->name));
         }
      }
      ast::Conditional *conditional = dynamic_cast<ast::Conditional *>(e);
      if (conditional)
      {
         ast::Expression *condition = resolve_exp(conditional->condition, variable_map, expressions);
         ast::Expression *left = resolve_exp(conditional->left, variable_map, expressions);
         ast::Expression *right = resolve_exp(conditional->right, variable_map, expressions);
         std::unique_ptr<ast::Conditional> c = std::make_unique<ast::Conditional>(condition, left, right);
         expressions.push_back(std::move(c));
         return expressions.back().get();
      }
      return e;
   }

   ast::Block *resolve_block(ast::Block *block, std::unordered_map<std::string, std::pair<std::string, bool>> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions){
      std::vector<std::unique_ptr<ast::Block_Item>> resolved_items;
      for (auto &item: block->block_items){
         ast::D *d = dynamic_cast<ast::D *>(item.get());
         ast::S *s = dynamic_cast<ast::S *>(item.get());

         if (d){
            ast::Declaration *resolved_declaration = resolve_declaration(d->declaration, variable_map, expressions);
            resolved_items.push_back(std::make_unique<ast::D>(resolved_declaration));
         }
         else if (s){
            ast::Statement *resolved_statement = resolve_statement(s->statement, variable_map, expressions);
            resolved_items.push_back(std::make_unique<ast::S>(resolved_statement));
         }
      }
      return new ast::Block(std::move(resolved_items));
   }

   ast::Declaration *resolve_declaration(ast::Declaration *declaration, std::unordered_map<std::string, std::pair<std::string, bool>> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      if (variable_map.count(declaration->name->name) && variable_map[declaration->name->name].second)
      {
         throw std::runtime_error(std::format("{} has already been declared.", declaration->name->name));
      }
      std::string unique_name = make_temporary(declaration->name->name);
      variable_map[declaration->name->name] = std::make_pair(unique_name, true);
      ast::Expression *prev_init = declaration->init;
      if (declaration->init)
      {
         prev_init = resolve_exp(declaration->init, variable_map, expressions);
      }
      ast::Declaration *resolved = new ast::Declaration(new ast::Identifier(unique_name), prev_init);
      return resolved;
   }

   ast::Statement *resolve_statement(ast::Statement *statement, std::unordered_map<std::string, std::pair<std::string, bool>> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      ast::Return *ret = dynamic_cast<ast::Return *>(statement);
      if (ret)
      {
         return new ast::Return(resolve_exp(ret->exp, variable_map, expressions));
      }
      ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(statement);
      if (exp_statement)
      {
         return new ast::Expression_Statement(resolve_exp(exp_statement->exp, variable_map, expressions));
      }
      ast::Null *null = dynamic_cast<ast::Null *>(statement);
      if (null)
      {
         return new ast::Null();
      }
      ast::If *if_statement = dynamic_cast<ast::If *>(statement);
      if (if_statement)
      {
         ast::Expression *condition = resolve_exp(if_statement->condition, variable_map, expressions);
         ast::Statement *then = resolve_statement(if_statement->then_statement, variable_map, expressions);
         ast::Statement *else_block = nullptr;
         if (if_statement->else_statement)
         {
            else_block = resolve_statement(if_statement->else_statement, variable_map, expressions);
         }
         return new ast::If(condition, then, else_block);
      }
      ast::Compound_Statement *compound_statement = dynamic_cast<ast::Compound_Statement *>(statement);
      if (compound_statement){
         std::unordered_map<std::string, std::pair<std::string, bool>> new_variable_map = copy_variable_map(variable_map);
         return new ast::Compound_Statement(resolve_block(compound_statement->block, new_variable_map, expressions));
      }
      ast::Break *break_statement = dynamic_cast<ast::Break *>(statement);
      if (break_statement) {
         return new ast::Break(make_label(break_statement->label->name));
      }
      ast::Continue *continue_statement = dynamic_cast<ast::Continue *>(statement);
      if (continue_statement) {
         return new ast::Continue(make_label(continue_statement->label->name));
      }
      ast::For *for_statement = dynamic_cast<ast::For *>(statement);
      if (for_statement) {
         std::unordered_map<std::string, std::pair<std::string, bool>> new_variable_map = copy_variable_map(variable_map);
         ast::For_Init *for_init = resolve_for_init(for_statement->init, new_variable_map, expressions);
         ast::Expression *condition = resolve_optional_exp(for_statement->condition, new_variable_map, expressions);
         ast::Expression *post = resolve_optional_exp(for_statement->post, new_variable_map, expressions);
         ast::Statement *body = resolve_statement(for_statement->body, new_variable_map, expressions);
         return new ast::For(for_init, body, make_label(for_statement->label->name), condition, post);
      }
      ast::While *while_statement = dynamic_cast<ast::While *>(statement);
      if (while_statement) {
         ast::Expression *condition = resolve_exp(while_statement->condition, variable_map, expressions);
         ast::Statement *body = resolve_statement(while_statement->body, variable_map, expressions);
         return new ast::While(condition, body, make_label(while_statement->label->name));
      }
      ast::DoWhile *do_while_statement = dynamic_cast<ast::DoWhile *>(statement);
      if (do_while_statement) {
         ast::Statement *body = resolve_statement(do_while_statement->body, variable_map, expressions);
         ast::Expression *condition = resolve_exp(do_while_statement->condition, variable_map, expressions);
         return new ast::DoWhile(body, condition, make_label(do_while_statement->label->name));
      }
      return nullptr;  
   }

   ast::For_Init *resolve_for_init(ast::For_Init *init, std::unordered_map<std::string, std::pair<std::string, bool>> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions) {
      ast::Init_Decl *init_decl = dynamic_cast<ast::Init_Decl *>(init);
      ast::Init_Exp *init_exp = dynamic_cast<ast::Init_Exp *>(init);
      if(init_decl) {
         return new ast::Init_Decl(resolve_declaration(init_decl->declaration, variable_map, expressions));
      } else if (init_exp) {
         return new ast::Init_Exp(resolve_optional_exp(init_exp->expression, variable_map, expressions));
      }
      return nullptr;
   }

   ast::Expression *resolve_optional_exp(ast::Expression *exp, std::unordered_map<std::string, std::pair<std::string, bool>> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions) {
      if (exp) {
         return resolve_exp(exp, variable_map, expressions);
      }
      return nullptr;
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
         ast::Identifier *copy = make_label(current_label->name);
         return annotate(break_statement, copy); 
      }
      ast::Continue *continue_statement = dynamic_cast<ast::Continue *>(statement);
      if (continue_statement) {
         if(!current_label) {
            throw std::runtime_error("Continue statement outside of loop.");
         }
         ast::Identifier *copy = make_label(current_label->name);
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

   // This function is hardcoded and handles the entire main function
   ast::Function *parse_function(std::list<std::string> &tokens)
   {
      expect("int", tokens);
      expect("main", tokens);
      ast::Identifier *func_name = new ast::Identifier("main");
      expect("(", tokens);
      expect("void", tokens);
      expect(")", tokens);
      expect("{", tokens);

      std::vector<std::unique_ptr<ast::Block_Item>> function_body;
      std::vector<std::unique_ptr<ast::Expression>> expressions;
      while (tokens.front() != "}")
      {
         std::unique_ptr<ast::Block_Item> next_block_item = parse_block_item(tokens, expressions);
         function_body.push_back(std::move(next_block_item));
      }
      ast::Block *body = new ast::Block(std::move(function_body));
      ast::Block *resolved_body = resolve_block(body, variable_map, expressions);
      delete body;
      body = label_block(resolved_body, nullptr);
      expect("}", tokens);
      ast::Function *func = new ast::Function(func_name, body, std::move(expressions));
      if (!tokens.empty())
      {
         throw std::runtime_error("Extra Characters Found For Minimal Compiler.");
      }

      variable_map.clear();
      symbol_table.clear();
      return func;
   }

   // This function converts Tokens into AST Program node
   ast::Program *parse(std::list<std::string> &tokens)
   {
      ast::Function *function_definition = parse_function(tokens);
      ast::Program *program = new ast::Program(function_definition);
      return program;
   }
}
