#include "ast.h"
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>

// This File is meant to convert the tokens into Abstract Syntax Tree nodes

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
Unary_Operator *parse_unop(std::list<std::string> &tokens)
{
  std::string next_token(*tokens.begin());
  tokens.erase(tokens.begin());
  if (next_token == "~"){
    Complement* complement = new Complement();
    return complement;
  }
  Negate* negate = new Negate();
  return negate;
}

// This recursive function Establishes Expression Nodes
Expression *parse_expression(std::list<std::string> &tokens)
{
  if (tokens.empty()) return nullptr; // REQUIRED BASE CASE
  std::string next_token(*tokens.begin());
  char *endptr;
  std::strtol(next_token.c_str(), &endptr, 10); // parses decimal integers
  if (*endptr == '\0') // valid integer
  {
    Constant *constant = new Constant(std::stoi(next_token));
    tokens.erase(tokens.begin());
    return constant;
  }
  else if (next_token == "~" || next_token == "-")
  {
    Unary_Operator* unary_operator = parse_unop(tokens);
    Expression* inner_exp = parse_expression(tokens); // A Unary Expression Can contain another Unary Expression Whitin
    Unary* unop = new Unary(unary_operator, inner_exp);
    return unop;
  }
  else if (next_token == "("){ // Separating Unary Operators
    tokens.erase(tokens.begin());
    Expression* inner_exp = parse_expression(tokens); 
    expect(")", tokens);
    return inner_exp;
  }
  else{
    std::cerr << "Malformed Expression" << std::endl;
    exit(1);
  }
}

// This function currently only handles return statements
Return *parse_statement(std::list<std::string> &tokens)
{
  expect("return", tokens);
  Expression *return_val = parse_expression(tokens);
  expect(";", tokens);
  Return *ret = new Return(return_val);
  return ret;
}

// This function is hardcoded and handles the entire main function
Function *parse_function(std::list<std::string> &tokens)
{
  expect("int", tokens);
  expect("main", tokens);
  Identifier *func_name = new Identifier("main");
  expect("(", tokens);
  expect("void", tokens);
  expect(")", tokens);
  expect("{", tokens);
  Function *func = new Function(func_name, parse_statement(tokens));
  expect("}", tokens);
  if (!tokens.empty())
  {
    std::cerr << "Extra Characters Found For Minimal Compiler" << std::endl;
    exit(1);
  }
  return func;
}

// This function is just to check AST structure
void pretty_print(const Program *p) { std::cout << *p << std::endl; } 

// This function converts Tokens into AST Program node
Program *parse(std::list<std::string> &tokens) 
{
  Function *function_definition = parse_function(tokens);
  Program *program = new Program(function_definition);
  // pretty_print(program);
  return program;
}
