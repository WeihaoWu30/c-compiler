#include "ast/ast.hpp"
#include "compiler/parser.hpp"
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <unordered_set>

// This File is meant to convert the tokens into Abstract Syntax Tree nodes

namespace parser
{
  // For Parsing Expressions
  std::unordered_set<std::string> binary_operators({"+", "-", "/", "%", "*", "<", "<=", ">", ">=", "==", "!=", "&&", "||"});

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

  // This function ONLY matches the next token against a bitwise negate or bitwise complement
  ast::Unary_Operator *parse_unop(std::list<std::string> &tokens)
  {
    std::string next_token(*tokens.begin());
    tokens.erase(tokens.begin());
    ast::Unary_Operator *res = nullptr;
    if (next_token == "~")
    {
      res = new ast::Complement();
    }
    else if (next_token == "-")
    {
      res = new ast::Negate();
    }
    else if (next_token == "!")
    {
      res = new ast::Unary_Not();
    }

    return res;
  }

  // This function creates AST nodes for binary operators
  ast::Binary_Operator *parse_binop(std::list<std::string> &tokens)
  {
    std::string next_token(*tokens.begin());
    tokens.erase(tokens.begin());
    ast::Binary_Operator *res = nullptr;
    if (next_token == "+")
    {
      res = new ast::Add();
    }
    else if (next_token == "-")
    {
      res = new ast::Subtract();
    }
    else if (next_token == "/")
    {
      res = new ast::Divide();
    }
    else if (next_token == "*")
    {
      res = new ast::Multiply();
    }
    else if (next_token == "%")
    {
      res = new ast::Remainder();
    }
    else if (next_token == "<")
    {
      res = new ast::LessThan();
    }
    else if (next_token == "<=")
    {
      res = new ast::LessOrEqual();
    }
    else if (next_token == ">")
    {
      res = new ast::GreaterThan();
    }
    else if (next_token == ">=")
    {
      res = new ast::GreaterOrEqual();
    }
    else if (next_token == "==")
    {
      res = new ast::Equal();
    }
    else if (next_token == "!=")
    {
      res = new ast::NotEqual();
    }
    else if (next_token == "&&")
    {
      res = new ast::And();
    }
    else if (next_token == "||")
    {
      res = new ast::Or();
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
    return 0;
  }

  // This function groups up expressions and orders them by precedence to create a recursive branch of Binary AST nodes
  ast::Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec)
  {
    ast::Expression *left = parse_factor(tokens);
    std::string next_token(*tokens.begin());
    while (binary_operators.count(next_token) && precedence(next_token) >= min_prec)
    { // Ensures that we process preceeding operators first
      ast::Binary_Operator *binary_operator = parse_binop(tokens);
      ast::Expression *right = parse_expression(tokens, precedence(next_token) + 1); // The +1 ensures we are grouping from the left side to the right side
      left = new ast::Binary(binary_operator, left, right);
      next_token = *tokens.begin();
    }

    return left;
  }

  // This recursive function Establishes Expression Nodes
  ast::Expression *parse_factor(std::list<std::string> &tokens)
  {
    if (tokens.empty())
      return nullptr; // REQUIRED BASE CASE
    std::string next_token(*tokens.begin());
    char *endptr;
    std::strtol(next_token.c_str(), &endptr, 10); // parses decimal integers
    if (*endptr == '\0')                          // valid integer
    {
      ast::Constant *constant = new ast::Constant(std::stoi(next_token));
      tokens.erase(tokens.begin());
      return constant;
    }
    else if (next_token == "~" || next_token == "-" || next_token == "!")
    {
      ast::Unary_Operator *unary_operator = parse_unop(tokens);
      ast::Expression *inner_exp = parse_factor(tokens); // A Unary Expression Can contain another Unary Expression Whitin
      ast::Unary *unop = new ast::Unary(unary_operator, inner_exp);
      return unop;
    }
    else if (next_token == "(")
    { // Separating Unary Operators
      tokens.erase(tokens.begin());
      ast::Expression *inner_exp = parse_expression(tokens, 0);
      expect(")", tokens);
      return inner_exp;
    }
    else
    {
      std::cerr << "Malformed Expression" << std::endl;
      exit(1);
    }
  }

  // This function currently only handles return statements
  ast::Return *parse_statement(std::list<std::string> &tokens)
  {
    expect("return", tokens);
    ast::Expression *return_val = parse_expression(tokens, 0);
    expect(";", tokens);
    ast::Return *ret = new ast::Return(return_val);
    return ret;
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
    ast::Function *func = new ast::Function(func_name, parse_statement(tokens));
    expect("}", tokens);
    if (!tokens.empty())
    {
      std::cerr << "Extra Characters Found For Minimal Compiler" << std::endl;
      exit(1);
    }
    return func;
  }

  // This function is just to check AST structure
  void pretty_print(const ast::Program *p) { std::cout << *p << std::endl; }

  // This function converts Tokens into AST Program node
  ast::Program *parse(std::list<std::string> &tokens)
  {
    ast::Function *function_definition = parse_function(tokens);
    ast::Program *program = new ast::Program(function_definition);
    // pretty_print(program);
    return program;
  }
}
