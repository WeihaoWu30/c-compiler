#include "ast/ast.hpp"
#include "compiler/parser.hpp"
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <unordered_set>
#include <iterator>
#include <compiler/ir_gen.hpp>
#include <memory>
#include <utility>

// This File is meant to convert the tokens into Abstract Syntax Tree nodes

namespace parser
{
   // For Parsing Expressions
   std::unordered_set<std::string> binary_operators({"+", "-", "/", "%", "*", "<", "<=", ">", ">=", "==", "!=", "&&", "||", "="});
   std::unordered_map<std::string, std::string> variable_map;

   // This Function Matches A Token Against Legal Syntax
   void expect(std::string expected, std::list<std::string> &tokens)
   {
      std::string actual(*tokens.begin());
      if (actual == expected)
      {
         tokens.erase(tokens.begin());
      }
      else
      {
         std::cerr << "Expected " << expected << " but found " << actual
                   << std::endl;
         exit(1);
      }
   }

   std::string make_temporary(std::string s)
   {
      return s + "." + std::to_string(ir_gen::id_counter);
   }

   // This function ONLY matches the next token against a bitwise negate or bitwise complement
   std::unique_ptr<ast::Unary_Operator> parse_unop(std::list<std::string> &tokens)
   {
      std::string next_token(*tokens.begin());
      tokens.erase(tokens.begin());
      std::unique_ptr<ast::Unary_Operator> res;
      if (next_token == "~")
      {
         res = std::make_unique<ast::Complement>();
      }
      else if (next_token == "-")
      {
         res = std::make_unique<ast::Negate>();
      }
      else if (next_token == "!")
      {
         res = std::make_unique<ast::Unary_Not>();
      }

      return res;
   }

   // This function creates AST nodes for binary operators
   std::unique_ptr<ast::Binary_Operator> parse_binop(std::list<std::string> &tokens)
   {
      std::string next_token(*tokens.begin());
      tokens.erase(tokens.begin());
      std::unique_ptr<ast::Binary_Operator> res;
      if (next_token == "+")
      {
         res = std::make_unique<ast::Add>();
      }
      else if (next_token == "-")
      {
         res = std::make_unique<ast::Subtract>();
      }
      else if (next_token == "/")
      {
         res = std::make_unique<ast::Divide>();
      }
      else if (next_token == "*")
      {
         res = std::make_unique<ast::Multiply>();
      }
      else if (next_token == "%")
      {
         res = std::make_unique<ast::Remainder>();
      }
      else if (next_token == "<")
      {
         res = std::make_unique<ast::LessThan>();
      }
      else if (next_token == "<=")
      {
         res = std::make_unique<ast::LessOrEqual>();
      }
      else if (next_token == ">")
      {
         res = std::make_unique<ast::GreaterThan>();
      }
      else if (next_token == ">=")
      {
         res = std::make_unique<ast::GreaterOrEqual>();
      }
      else if (next_token == "==")
      {
         res = std::make_unique<ast::Equal>();
      }
      else if (next_token == "!=")
      {
         res = std::make_unique<ast::NotEqual>();
      }
      else if (next_token == "&&")
      {
         res = std::make_unique<ast::And>();
      }
      else if (next_token == "||")
      {
         res = std::make_unique<ast::Or>();
      }

      return res;
   }

   // This function hard codes each symbol's precedence values
   uint16_t precedence(const std::string &next_token)
   {
      if (next_token == "+" || next_token == "-")
         return 45;
      else if (next_token == "*" || next_token == "/" || next_token == "%")
         return 50;
      else if (next_token == "<" || next_token == "<=" || next_token == ">" || next_token == ">=")
         return 35;
      else if (next_token == "==" || next_token == "!=")
         return 30;
      else if (next_token == "&&")
         return 10;
      else if (next_token == "||")
         return 5;
      else if (next_token == "=")
         return 1;
      return 0;
   }

   // This function groups up expressions and orders them by precedence to create a recursive branch of Binary AST nodes
   std::unique_ptr<ast::Expression> parse_expression(std::list<std::string> &tokens, uint16_t min_prec)
   {
      std::unique_ptr<ast::Expression> left = parse_factor(tokens);
      std::string next_token(*tokens.begin());
      std::unique_ptr<ast::Expression> right;
      while (binary_operators.count(next_token) && precedence(next_token) >= min_prec)
      { // Ensures that we process preceeding operators first
         if (next_token == "=")
         {
            expect("=", tokens);
            right = parse_expression(tokens, precedence(next_token));
            left = std::make_unique<ast::Assignment>(left, right);
         }
         else
         {
            std::unique_ptr<ast::Binary_Operator> binary_operator = parse_binop(tokens);
            right = parse_expression(tokens, precedence(next_token) + 1); // The +1 ensures we are grouping from the left side to the right side
            left = std::make_unique<ast::Binary>(std::move(binary_operator), std::move(left), std::move(right));
         }
         next_token = *tokens.begin();
      }

      return left;
   }

   // This recursive function Establishes Expression Nodes
   std::unique_ptr<ast::Expression> parse_factor(std::list<std::string> &tokens)
   {
      if (tokens.empty())
         return nullptr; // REQUIRED BASE CASE
      std::string next_token(*tokens.begin());
      char *endptr;
      std::strtol(next_token.c_str(), &endptr, 10); // parses decimal integers
      if (*endptr == '\0')                          // valid integer
      {
         std::unique_ptr<ast::Constant> constant = std::make_unique<ast::Constant>(std::stoi(next_token));
         tokens.erase(tokens.begin());
         return constant;
      }
      else if (next_token == "~" || next_token == "-" || next_token == "!")
      {
         std::unique_ptr<ast::Unary_Operator> unary_operator = parse_unop(tokens);
         std::unique_ptr<ast::Expression> inner_exp = parse_factor(tokens); // A Unary Expression Can contain another Unary Expression Whitin
         std::unique_ptr<ast::Unary> unop = std::make_unique<ast::Unary>(std::move(unary_operator), std::move(inner_exp));
         return unop;
      }
      else if (next_token == "(")
      { // Separating Unary Operators
         tokens.erase(tokens.begin());
         std::unique_ptr<ast::Expression> inner_exp = parse_expression(tokens, 0);
         expect(")", tokens);
         return inner_exp;
      }
      else if (binary_operators.count(tokens.front()) == 0 && tokens.front() != "int")
      {
         std::unique_ptr<ast::Var> variable = std::make_unique<ast::Var>(std::make_shared<ast::Identifier>(tokens.front()));
         tokens.erase(tokens.begin());
         return variable;
      }
      else
      {
         std::cerr << "Malformed Expression " << *tokens.begin() << std::endl;
         exit(1);
      }
   }

   // This function currently only handles return statements
   std::unique_ptr<ast::Statement> parse_statement(std::list<std::string> &tokens)
   {
      if (tokens.front() == "return")
      {
         expect("return", tokens);
         std::unique_ptr<ast::Expression> return_val = parse_expression(tokens, 0);
         expect(";", tokens);
         std::unique_ptr<ast::Return> ret = std::make_unique<ast::Return>(return_val);
         return ret;
      }
      else if (tokens.front() == ";")
      {
         expect(";", tokens);
         std::unique_ptr<ast::Null> null_statement = std::make_unique<ast::Null>();
         return null_statement;
      }
      else
      {
         std::unique_ptr<ast::Expression> exp = parse_expression(tokens, 0);
         std::unique_ptr<ast::Expression_Statement> exp_statement = std::make_unique<ast::Expression_Statement>(exp);
         expect(";", tokens);
         return exp_statement;
      }
      return nullptr;
   }

   std::unique_ptr<ast::Expression> resolve_exp(std::unique_ptr<ast::Expression> e)
   {
      ast::Assignment *assignment = dynamic_cast<ast::Assignment *>(e.get());
      ast::Var *var = dynamic_cast<ast::Var *>(e.get());
      if (assignment)
      {
         ast::Var *var_node = dynamic_cast<ast::Var *>(assignment->lvalue.get());
         if (!var_node)
         {
            std::cerr << "Invalid lvalue" << std::endl;
         }
         return std::make_unique<ast::Assignment>(resolve_exp(std::move(assignment->lvalue)), resolve_exp(std::move(assignment->exp)));
      }
      else if (var)
      {
         if (variable_map.count(var->identifier->name))
         {
            return std::make_unique<ast::Var>(std::make_shared<ast::Identifier>(variable_map[var->identifier->name]));
         }
         else
         {
            std::cerr << "Undeclared variable!" << std::endl;
         }
      }
      return nullptr;
   }

   std::unique_ptr<ast::Declaration> resolve_declaration(std::unique_ptr<ast::Declaration> declaration)
   {
      if (variable_map.count(declaration->name->name) == 1)
      {
         std::cerr << "Duplicate variable declaration" << std::endl;
         exit(1);
      }
      std::string unique_name = make_temporary(declaration->name->name);
      variable_map[declaration->name->name] = unique_name;
      if (declaration->init)
      {
         declaration->init = resolve_exp(std::move(declaration->init));
      }
      return std::make_unique<ast::Declaration>(std::make_shared<ast::Identifier>(unique_name), declaration->init);
   }

   std::unique_ptr<ast::Statement> resolve_statement(std::unique_ptr<ast::Statement> statement)
   {
      ast::Return *ret = dynamic_cast<ast::Return *>(statement.get());
      ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(statement.get());
      ast::Null *null = dynamic_cast<ast::Null *>(statement.get());
      if (ret)
      {
         return std::make_unique<ast::Return>(resolve_exp(std::move(ret->exp)));
      }
      if (exp_statement)
      {
         return std::make_unique<ast::Expression_Statement>(resolve_exp(std::move(exp_statement->exp)));
      }
      if (null)
      {
         return std::make_unique<ast::Null>();
      }
      return nullptr;
   }

   std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string> &tokens)
   {
      // std::cout << tokens.front() << std::endl;
      std::unique_ptr<ast::Var> variable;
      std::unique_ptr<ast::Declaration> declaration;
      if (tokens.front() == "int")
      {
         tokens.erase(tokens.begin());
         char *endptr;
         std::strtol(tokens.front().c_str(), &endptr, 10); // parses decimal integers
         if (*endptr != '\0' && tokens.front() != "return")
         {
            std::string name(tokens.front());
            tokens.erase(tokens.begin());
            std::shared_ptr<ast::Identifier> identifier = std::make_shared<ast::Identifier>(name);
            variable = std::make_unique<ast::Var>(identifier); // move identifier into variable
         }
         else
         {
            std::cerr << "Not a valid variable name" << std::endl;
            exit(1);
         }
         if (tokens.front() == "=")
         {
            expect("=", tokens);
            std::unique_ptr<ast::Expression> exp = std::make_unique<ast::Expression>(parse_expression(tokens, 0));
            declaration = std::make_unique<ast::Declaration>(variable->identifier, exp); // shares identifier with Var
            expect(";", tokens);
         }
         else if (tokens.front() == ";")
         {
            declaration = std::make_unique<ast::Declaration>(variable->identifier);
            expect(";", tokens);
         }
         else
         {
            std::cerr << "Not a valid block item" << std::endl;
            exit(1);
         }
         std::unique_ptr<ast::D> d = std::make_unique<ast::D>(std::move(declaration));
         return d;
      }
      else
      {
         // std::cout << tokens.front() << std::endl;
         std::unique_ptr<ast::S> s = std::make_unique<ast::S>(parse_statement(tokens));
         return s;
      }
   }

   // This function is hardcoded and handles the entire main function
   std::unique_ptr<ast::Function> parse_function(std::list<std::string> &tokens)
   {
      expect("int", tokens);
      expect("main", tokens);
      std::unique_ptr<ast::Identifier> func_name = std::make_unique<ast::Identifier>("main");
      expect("(", tokens);
      expect("void", tokens);
      expect(")", tokens);
      expect("{", tokens);

      std::list<std::unique_ptr<ast::Block_Item>> function_body;
      while (tokens.front() != "}")
      {
         std::unique_ptr<ast::Block_Item> next_block_item = parse_block_item(tokens);
         function_body.push_back(next_block_item);
      }
      expect("}", tokens);
      if (!tokens.empty())
      {
         std::cerr << "Extra Characters Found For Minimal Compiler" << std::endl;
         exit(1);
      }
      std::unique_ptr<ast::Function> function = std::make_unique<ast::Function>(func_name, function_body);
      return function;
   }

   // This function is just to check AST structure
   void pretty_print(std::unique_ptr<ast::Program> p) { std::cout << *p << std::endl; }

   // This function converts Tokens into AST Program node
   std::unique_ptr<ast::Program> parse(std::list<std::string> &tokens)
   {
      std::unique_ptr<ast::Function> function_definition = parse_function(tokens);
      std::unique_ptr<ast::Program> program = std::make_unique<ast::Program>(std::move(function_definition));
      // pretty_print(std::move(program));
      return program;
   }
}
