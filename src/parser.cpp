#include "ast/ast.hpp"
#include "compiler/parser.hpp"
#include <cstdlib>
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
   std::unordered_map<std::string, std::string> variable_map;
   std::unordered_map<std::string, std::string> symbol_table;

   // This Function Matches A Token Against Legal Syntax
   void expect(std::string expected, std::list<std::string> &tokens)
   {
      std::string actual(tokens.front());
      if (actual == expected)
      {
         tokens.pop_front();
      }
      else
      {
         std::cerr << "Expected " << expected << " but found " << actual
                   << std::endl;
         exit(1);
      }
   }

   bool is_type(const std::string &token)
   {
      return token == "int" || symbol_table.count(token);
   }

   std::string make_temporary(std::string s)
   {
      return s + "." + std::to_string(ir_gen::id_counter);
   }

   // This function ONLY matches the next token against a bitwise negate or bitwise complement
   ast::Unary_Operator parse_unop(std::list<std::string> &tokens)
   {
      std::string next_token(tokens.front());
      tokens.pop_front();
      if (next_token == "~")
      {
         return ast::Unary_Operator::Complement;
      }
      else if (next_token == "-")
      {
         return ast::Unary_Operator::Negate;
      }
      else if (next_token == "!")
      {
         return ast::Unary_Operator::Not;
      }

      return ast::Unary_Operator::Invalid;
   }

   // This function creates AST nodes for binary operators
   ast::Binary_Operator parse_binop(std::list<std::string> &tokens)
   {
      std::string next_token(tokens.front());
      tokens.pop_front();
      if (next_token == "+")
      {
         return ast::Binary_Operator::Add;
      }
      else if (next_token == "-")
      {
         return ast::Binary_Operator::Subtract;
      }
      else if (next_token == "/")
      {
         return ast::Binary_Operator::Divide;
      }
      else if (next_token == "*")
      {
         return ast::Binary_Operator::Multiply;
      }
      else if (next_token == "%")
      {
         return ast::Binary_Operator::Remainder;
      }
      else if (next_token == "<")
      {
         return ast::Binary_Operator::LessThan;
      }
      else if (next_token == "<=")
      {
         return ast::Binary_Operator::LessOrEqual;
      }
      else if (next_token == ">")
      {
         return ast::Binary_Operator::GreaterThan;
      }
      else if (next_token == ">=")
      {
         return ast::Binary_Operator::GreaterOrEqual;
      }
      else if (next_token == "==")
      {
         return ast::Binary_Operator::Equal;
      }
      else if (next_token == "!=")
      {
         return ast::Binary_Operator::NotEqual;
      }
      else if (next_token == "&&")
      {
         return ast::Binary_Operator::And;
      }
      else if (next_token == "||")
      {
         return ast::Binary_Operator::Or;
      }
      else if (next_token == "|")
      {
         return ast::Binary_Operator::BitOr;
      }
      else if (next_token == "&")
      {
         return ast::Binary_Operator::BitAnd;
      }
      else if (next_token == "^")
      {
         return ast::Binary_Operator::BitXor;
      }
      else if (next_token == ">>")
      {
         return ast::Binary_Operator::BitRightShift;
      }
      else if (next_token == "<<")
      {
         return ast::Binary_Operator::BitLeftShift;
      }

      return ast::Binary_Operator::Invalid;
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
      else if (next_token == "=")
         return 1;
      return 0;
   }

   // This function groups up expressions and orders them by precedence to create a recursive branch of Binary AST nodes
   ast::Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      ast::Expression *left = parse_factor(tokens, expressions);
      std::string next_token(tokens.front());
      ast::Expression *right;
      while (std::find(binary_operators.begin(), binary_operators.end(), next_token) != binary_operators.end() && precedence(next_token) >= min_prec)
      { // Ensures that we process preceeding operators first
         if (next_token == "=")
         {
            expect("=", tokens);
            right = parse_expression(tokens, precedence(next_token), expressions);
            std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Assignment>(left, right);
            left = unique_left.get();
            expressions.push_back(std::move(unique_left));
         }
         else
         {
            ast::Binary_Operator binary_operator = parse_binop(tokens);
            ast::Expression *right = parse_expression(tokens, precedence(next_token) + 1, expressions); // The +1 ensures we are grouping from the left side to the right side
            std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Binary>(binary_operator, left, right);
            expressions.push_back(std::move(unique_left));
            left = expressions.back().get();
         }
         next_token = tokens.front();
      }

      return left;
   }

   // This recursive function Establishes Expression Nodes
   ast::Expression *parse_factor(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      if (tokens.empty())
         return nullptr; // REQUIRED BASE CASE
      std::string next_token(tokens.front());
      char *endptr;
      std::strtol(next_token.c_str(), &endptr, 10); // parses decimal integers
      if (*endptr == '\0')                          // valid integer
      {
         std::unique_ptr<ast::Constant> constant = std::make_unique<ast::Constant>(std::stoi(next_token));
         expressions.push_back(std::move(constant));
         tokens.pop_front();
         return expressions.back().get();
      }
      else if (next_token == "~" || next_token == "-" || next_token == "!")
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
      else if (std::find(binary_operators.begin(), binary_operators.end(), next_token) == binary_operators.end() && tokens.front() != "int")
      {
         std::unique_ptr<ast::Var> variable = std::make_unique<ast::Var>(new ast::Identifier(next_token));
         expressions.push_back(std::move(variable));
         tokens.pop_front();
         return expressions.back().get();
      }
      else
      {
         // ast::Identifier *identifier = new ast::Identifier(tokens.front());
         // ast::Var *variable = new ast::Var(identifier);
         // tokens.erase(tokens.begin());
         // return variable;
         std::cerr << "Malformed Expression " << *tokens.begin() << std::endl;
         exit(1);
      }
   }

   // This function currently only handles return statements
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
      else if (tokens.front() == ";")
      {
         expect(";", tokens);
         ast::Null *null_statement = new ast::Null();
         return null_statement;
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

   ast::Expression *resolve_exp(ast::Expression *e, std::unordered_map<std::string, std::string> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      ast::Assignment *assignment = dynamic_cast<ast::Assignment *>(e);
      ast::Binary *binary = dynamic_cast<ast::Binary *>(e);
      ast::Unary *unary = dynamic_cast<ast::Unary *>(e);
      ast::Var *var = dynamic_cast<ast::Var *>(e);
      if (assignment)
      {
         ast::Var *var_node = dynamic_cast<ast::Var *>(assignment->lvalue);
         if (!var_node)
         {
            std::cerr << "Invalid lvalue" << std::endl;
            exit(1);
         }
         ast::Expression *lvalue = resolve_exp(assignment->lvalue, variable_map, expressions);
         ast::Expression *exp = resolve_exp(assignment->exp, variable_map, expressions);
         std::unique_ptr<ast::Assignment> a = std::make_unique<ast::Assignment>(lvalue, exp);
         expressions.push_back(std::move(a));
         return expressions.back().get();
      }
      else if (binary)
      {
         ast::Expression *left = resolve_exp(binary->left, variable_map, expressions);
         ast::Expression *right = resolve_exp(binary->right, variable_map, expressions);
         std::unique_ptr<ast::Binary> b = std::make_unique<ast::Binary>(binary->binary_operator, left, right);
         expressions.push_back(std::move(b));
         return expressions.back().get();
      }
      else if (unary)
      {
         ast::Expression *exp = resolve_exp(unary->exp, variable_map, expressions);
         std::unique_ptr<ast::Unary> u = std::make_unique<ast::Unary>(unary->unary_operator, exp);
         expressions.push_back(std::move(u));
         return expressions.back().get();
      }
      else if (var)
      {
         if (variable_map.count(var->identifier->name))
         {
            std::unique_ptr<ast::Var> v = std::make_unique<ast::Var>(new ast::Identifier(variable_map[var->identifier->name]));
            expressions.push_back(std::move(v));
            return expressions.back().get();
         }
         else
         {
            std::cerr << "Undeclared variable!" << std::endl;
            exit(1);
         }
      }
      return e;
   }

   ast::Declaration *resolve_declaration(ast::Declaration *declaration, std::unordered_map<std::string, std::string> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      if (variable_map.count(declaration->name->name) == 1)
      {
         std::cerr << "Duplicate variable declaration" << std::endl;
         exit(1);
      }
      std::string unique_name = make_temporary(declaration->name->name);
      variable_map[declaration->name->name] = unique_name;
      ast::Expression *prev_init = declaration->init;
      if (declaration->init != nullptr)
      {
         prev_init = resolve_exp(declaration->init, variable_map, expressions);
      }
      ast::Declaration *resolved = new ast::Declaration(new ast::Identifier(unique_name), prev_init);
      return resolved;
   }

   ast::Statement *resolve_statement(ast::Statement *statement, std::unordered_map<std::string, std::string> &variable_map, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      ast::Return *ret = dynamic_cast<ast::Return *>(statement);
      ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(statement);
      ast::Null *null = dynamic_cast<ast::Null *>(statement);
      if (ret)
      {
         return new ast::Return(resolve_exp(ret->exp, variable_map, expressions));
      }
      if (exp_statement)
      {
         return new ast::Expression_Statement(resolve_exp(exp_statement->exp, variable_map, expressions));
      }
      if (null)
      {
         return new ast::Null();
      }
      return nullptr;
   }

   std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string> &tokens, std::vector<std::unique_ptr<ast::Expression>> &expressions)
   {
      // handling typedef
      if (tokens.front() == "typedef")
      {
         tokens.pop_front();
         if (!is_type(tokens.front()))
         {
            std::cerr << "Not a valid type" << std::endl;
            exit(1);
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
         ast::Identifier *variable_name;
         ast::Declaration *declaration;
         char *endptr;
         std::strtol(tokens.front().c_str(), &endptr, 10); // parses decimal integers
         if (std::next(tokens.begin()) == tokens.end())
         {
            std::cerr << "Missing semicolon" << std::endl;
            exit(1);
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
            std::cerr << "Not a valid variable name" << std::endl;
            exit(1);
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
            std::cerr << "Not a valid block item" << std::endl;
            exit(1);
         }
         ast::Declaration *resolved_declaration = resolve_declaration(declaration, variable_map, expressions);
         std::unique_ptr<ast::D> d = std::make_unique<ast::D>(resolved_declaration);
         delete declaration;
         return d;
      }
      else
      {
         // std::cout << tokens.front() << std::endl;
         ast::Statement *unresolved_statement = parse_statement(tokens, expressions);
         ast::Statement *resolved_statement = resolve_statement(unresolved_statement, variable_map, expressions);
         std::unique_ptr<ast::S> s = std::make_unique<ast::S>(resolved_statement);
         delete unresolved_statement;
         return s;
      }
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
      expect("}", tokens);
      ast::Function *func = new ast::Function(func_name, std::move(function_body), std::move(expressions));
      if (!tokens.empty())
      {
         std::cerr << "Extra Characters Found For Minimal Compiler" << std::endl;
         exit(1);
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
