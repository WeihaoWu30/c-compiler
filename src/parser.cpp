#include "ast.h"
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>

void expect(std::string expected, std::list<std::string> &tokens) {
  std::string actual(*tokens.begin());
  if (actual == expected) {
    tokens.erase(tokens.begin());
  } else {
    std::cerr << "Expected " << expected << " but found " << actual
              << std::endl;
    exit(1);
  }
}

Constant *parse_expression(std::list<std::string> &tokens) {
  Constant *exp = new Constant(std::stoi(*tokens.begin()));
  tokens.erase(tokens.begin());
  return exp;
}

Return *parse_statement(std::list<std::string> &tokens) {
  expect("return", tokens);
  Constant *return_val = parse_expression(tokens);
  expect(";", tokens);
  Return *ret = new Return(return_val);
  return ret;
}

Function *parse_function(std::list<std::string> &tokens) {
  expect("int", tokens);
  expect("main", tokens);
  Identifier *func_name = new Identifier("main");
  expect("(", tokens);
  expect("void", tokens);
  expect(")", tokens);
  expect("{", tokens);
  Function *func = new Function(func_name, parse_statement(tokens));
  expect("}", tokens);
  if (!tokens.empty()) {
    std::cerr << "Extra Characters Found For Minimal Compiler" << std::endl;
    exit(1);
  }
  return func;
}

void pretty_print(const Program *p) { std::cout << *p << std::endl; }

Program *parse(std::list<std::string> &tokens) {
  Function *function_definition = parse_function(tokens);
  Program *program = new Program(function_definition);
  pretty_print(program);
  return program;
}
