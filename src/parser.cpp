#include "ast.h"
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <unordered_set>

// This File is meant to convert the tokens into Abstract Syntax Tree nodes

// Prototyping
void expect(std::string expected, std::list<std::string> &tokens);
Unary_Operator *parse_unop(std::list<std::string> &tokens);
Binary_Operator *parse_binop(std::list<std::string> &tokens);
uint16_t precedence(const std::string& next_token);
Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec);
Expression *parse_factor(std::list<std::string> &tokens);
Return *parse_statement(std::list<std::string> &tokens);
Function *parse_function(std::list<std::string> &tokens);
void pretty_print(const Program *p);
Program *parse(std::list<std::string> &tokens);


// For Parsing Expressions
std::unordered_set<std::string> binary_operators({ "+", "-", "/", "%", "*", "<", "<=", ">", ">=", "==", "!=", "&&", "||" });

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
  Unary_Operator *res = nullptr;
  if (next_token == "~"){
    res = new Complement();
  } else if(next_token == "-") {
    res = new Negate();
  } else if (next_token == "!") {
    res = new Unary_Not();
  }

  return res;
}

// This function creates AST nodes for binary operators
Binary_Operator *parse_binop(std::list<std::string> &tokens) {
  std::string next_token(*tokens.begin());
  tokens.erase(tokens.begin());
  Binary_Operator *res = nullptr;
  if(next_token == "+") {
    res = new Add();
  } else if(next_token == "-") {
    res = new Subtract();
  } else if(next_token == "/") {
    res = new Divide();
  } else if(next_token == "*") {
    res = new Multiply();
  } else if(next_token == "%") {
    res = new Remainder();
  } else if(next_token == "<"){
    res = new LessThan();
  } else if(next_token == "<="){
    res = new LessOrEqual();
  } else if(next_token == ">"){
    res = new GreaterThan();
  } else if (next_token == ">="){
    res = new GreaterOrEqual();
  } else if (next_token == "=="){
    res = new Equal();
  } else if (next_token == "!="){
    res = new NotEqual();
  } else if (next_token == "&&"){
    res = new And();
  } else if (next_token == "||"){
    res = new Or();
  }

  return res;
}

// This function hard codes each symbol's precedence values
uint16_t precedence(const std::string& next_token) {
  if(next_token == "+" || next_token == "-") return 45;
  else if(next_token == "*" || next_token == "/" || next_token == "%") return 50;
  else if (next_token == "<" || next_token == "<=" || next_token == ">" || next_token == ">=") return 35;
  else if (next_token == "==" || next_token == "!=") return 30;
  else if (next_token == "&&") return 10;
  else if (next_token == "||") return 5;
  return 0;
}

// This function groups up expressions and orders them by precedence to create a recursive branch of Binary AST nodes
Expression *parse_expression(std::list<std::string> &tokens, uint16_t min_prec) {
  Expression *left = parse_factor(tokens);
  std::string next_token(*tokens.begin());
  while(binary_operators.count(next_token) && precedence(next_token) >= min_prec) { // Ensures that we process preceeding operators first
    Binary_Operator *binary_operator = parse_binop(tokens);
    Expression *right = parse_expression(tokens, precedence(next_token) + 1); // The +1 ensures we are grouping from the left side to the right side
    left = new Binary(binary_operator, left, right);
    next_token = *tokens.begin();
  }

  return left;
}

// This recursive function Establishes Expression Nodes
Expression *parse_factor(std::list<std::string> &tokens)
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
  else if (next_token == "~" || next_token == "-" || next_token == "!")
  {
    Unary_Operator* unary_operator = parse_unop(tokens);
    Expression* inner_exp = parse_factor(tokens); // A Unary Expression Can contain another Unary Expression Whitin
    Unary* unop = new Unary(unary_operator, inner_exp);
    return unop;
  }
  else if (next_token == "("){ // Separating Unary Operators
    tokens.erase(tokens.begin());
    Expression* inner_exp = parse_expression(tokens, 0); 
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
  Expression *return_val = parse_expression(tokens, 0);
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
