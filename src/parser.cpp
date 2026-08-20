#include "compiler/parser.hpp"
#include "ast/ast.hpp"
#include "ast/identifier_attrs/identifier_attr.hpp"
#include "ast/storage_class/storage_classes.hpp"
#include "compiler/tools.hpp"
#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <iterator>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// This File is meant to convert the tokens into Abstract Syntax Tree nodes

namespace parser {
  // For Parsing Expressions
  std::unordered_map<std::string, std::string> type_aliases; // maps types to typedef aliases

  // This Function Matches A Token Against Legal Syntax
  void expect(std::string expected, std::list<std::string>& tokens) {
    if (tokens.empty()) { throw std::runtime_error(std::format("Expected {}, but found nothing.", expected)); }
    std::string actual(tokens.front());
    if (actual == expected) {
      tokens.pop_front();
    } else {
      throw std::runtime_error(std::format("Expected {} but found {}.", expected, actual));
    }
  }

  // This function ONLY matches the next token against a bitwise negate or bitwise complement
  ast::Unary_Operator parse_unop(std::list<std::string>& tokens) {
    std::string next_token(tokens.front());
    tokens.pop_front();
    if (next_token == "~") return ast::Unary_Operator::Complement;
    else if (next_token == "-") return ast::Unary_Operator::Negate;
    else if (next_token == "!") return ast::Unary_Operator::Not;
    return ast::Unary_Operator::Invalid;
  }

  // This function creates AST nodes for binary operators
  ast::Binary_Operator parse_binop(std::list<std::string>& tokens) {
    std::string next_token(tokens.front());
    tokens.pop_front();
    if (next_token == "+") return ast::Binary_Operator::Add;
    else if (next_token == "-") return ast::Binary_Operator::Subtract;
    else if (next_token == "/") return ast::Binary_Operator::Divide;
    else if (next_token == "*") return ast::Binary_Operator::Multiply;
    else if (next_token == "%") return ast::Binary_Operator::Remainder;
    else if (next_token == "<") return ast::Binary_Operator::LessThan;
    else if (next_token == "<=") return ast::Binary_Operator::LessOrEqual;
    else if (next_token == ">") return ast::Binary_Operator::GreaterThan;
    else if (next_token == ">=") return ast::Binary_Operator::GreaterOrEqual;
    else if (next_token == "==") return ast::Binary_Operator::Equal;
    else if (next_token == "!=") return ast::Binary_Operator::NotEqual;
    else if (next_token == "&&") return ast::Binary_Operator::And;
    else if (next_token == "||") return ast::Binary_Operator::Or;
    else if (next_token == "|") return ast::Binary_Operator::BitOr;
    else if (next_token == "&") return ast::Binary_Operator::BitAnd;
    else if (next_token == "^") return ast::Binary_Operator::BitXor;
    else if (next_token == ">>") return ast::Binary_Operator::BitRightShift;
    else if (next_token == "<<") return ast::Binary_Operator::BitLeftShift;
    return ast::Binary_Operator::Invalid;
  }

  // Parses only compound operators for right association
  ast::Compound_Operator parse_comop(std::list<std::string>& tokens) {
    std::string next_token(tokens.front());
    tokens.pop_front();
    if (next_token == "+=") return ast::Compound_Operator::AdditionAssignment;
    else if (next_token == "-=") return ast::Compound_Operator::SubtractionAssignment;
    else if (next_token == "*=") return ast::Compound_Operator::MultiplicationAssignment;
    else if (next_token == "/=") return ast::Compound_Operator::DivisionAssignment;
    else if (next_token == "%=") return ast::Compound_Operator::ModulusAssignment;
    else if (next_token == "&=") return ast::Compound_Operator::BitwiseAndAssignment;
    else if (next_token == "|=") return ast::Compound_Operator::BitwiseOrAssignment;
    else if (next_token == "^=") return ast::Compound_Operator::BitwiseXorAssignment;
    else if (next_token == ">>=") return ast::Compound_Operator::RightShiftAssignment;
    else if (next_token == "<<=") return ast::Compound_Operator::LeftShiftAssignment;
    return ast::Compound_Operator::Invalid;
  }

  // This function hard codes each symbol's precedence values
  uint16_t precedence(const std::string& next_token) {
    if (next_token == "*" || next_token == "/" || next_token == "%") return 50;
    else if (next_token == "+" || next_token == "-") return 45;
    else if (next_token == "<<" || next_token == ">>") return 40;
    else if (next_token == "<" || next_token == "<=" || next_token == ">" || next_token == ">=") return 35;
    else if (next_token == "==" || next_token == "!=") return 30;
    else if (next_token == "&") return 25;
    else if (next_token == "^") return 20;
    else if (next_token == "|") return 15;
    else if (next_token == "&&") return 10;
    else if (next_token == "||") return 5;
    else if (next_token == "?") return 3;
    else if (next_token == "=" || (std::find(compound_operators.begin(), compound_operators.end(), next_token) != compound_operators.end())) return 1;
    return 0;
  }

  ast::Expression* parse_conditional_middle(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions) {
    expect("?", tokens);
    ast::Expression* middle = parse_expression(tokens, 0, expressions);
    expect(":", tokens);
    return middle;
  }

  // This function groups up expressions and orders them by precedence to create a recursive branch of Binary AST nodes
  ast::Expression* parse_expression(std::list<std::string>& tokens, uint16_t min_prec, std::vector<std::unique_ptr<ast::Expression>>& expressions) {
    ast::Expression* left = parse_factor(tokens, expressions);
    if (tokens.empty()) { throw std::runtime_error("Expected ;, but found nothing."); }
    std::string next_token(tokens.front());
    ast::Expression* right;
    while ((std::find(binary_operators.begin(), binary_operators.end(), next_token) != binary_operators.end() ||
            std::find(compound_operators.begin(), compound_operators.end(), next_token) != compound_operators.end()) &&
           precedence(next_token) >= min_prec) { // Ensures that we process preceeding operators first
      if (next_token == "=") {
        expect("=", tokens);
        right = parse_expression(tokens, precedence(next_token), expressions);
        std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Assignment>(left, right);
        expressions.push_back(std::move(unique_left));
      } else if (std::find(compound_operators.begin(), compound_operators.end(), next_token) != compound_operators.end()) {
        ast::Compound_Operator compound_operator = parse_comop(tokens);
        right = parse_expression(tokens, precedence(next_token), expressions);
        std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Compound>(compound_operator, left, right);
        expressions.push_back(std::move(unique_left));
      } else if (next_token == "?") {
        ast::Expression* middle = parse_conditional_middle(tokens, expressions);
        right = parse_expression(tokens, precedence(next_token), expressions);
        std::unique_ptr<ast::Expression> unique_left = std::make_unique<ast::Conditional>(left, middle, right);
        expressions.push_back(std::move(unique_left));
      } else {
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
  ast::Expression* parse_factor(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions) {
    if (tokens.empty()) { throw std::runtime_error("Expected expression after operator, but found nothing."); }
    std::string next_token(tokens.front());
    char* endptr;
    long value = std::strtol(next_token.c_str(), &endptr, 10); // parses decimal integers
    if (*endptr == '\0')                                       // valid integer
    {
      std::unique_ptr<ast::Constant> constant = std::make_unique<ast::Constant>(value);
      expressions.push_back(std::move(constant));
      tokens.pop_front();
      return expressions.back().get();
    } else if (std::find(unary_operators.begin(), unary_operators.end(), next_token) != unary_operators.end()) {
      ast::Unary_Operator unary_operator = parse_unop(tokens);
      ast::Expression* inner_exp = parse_factor(tokens, expressions); // A Unary Expression Can contain another Unary Expression Whitin
      std::unique_ptr<ast::Unary> unop = std::make_unique<ast::Unary>(unary_operator, inner_exp);
      expressions.push_back(std::move(unop));
      return expressions.back().get();
    } else if (next_token == "(") { // Separating Unary Operators
      tokens.pop_front();
      ast::Expression* inner_exp = parse_expression(tokens, 0, expressions);
      expect(")", tokens);
      return inner_exp;
    } else if ( // find std method that combines these
        std::find(binary_operators.begin(), binary_operators.end(), next_token) == binary_operators.end() &&
        std::find(compound_operators.begin(), compound_operators.end(), next_token) == compound_operators.end() &&
        std::find(unary_operators.begin(), unary_operators.end(), next_token) == unary_operators.end() && std::find(specifiers.begin(), specifiers.end(), next_token) == specifiers.end()) {
      ast::Identifier* name = new ast::Identifier(next_token);
      tokens.pop_front();
      if (tokens.front() == "(") {
        tokens.pop_front();
        std::vector<ast::Expression*> arguemnt_list;
        while (!tokens.empty() && tokens.front() != ")") {
          ast::Expression* arguement = parse_expression(tokens, 0, expressions);
          arguemnt_list.push_back(std::move(arguement));
          if (tokens.front() == ")") break;
          else {
            expect(",", tokens);
            if (!tokens.empty() && tokens.front() == ")") { throw std::runtime_error(std::format("Trailing comma in argument list of function call {}.", name->text)); }
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
    } else {
      throw std::runtime_error(std::format("Malformed Expression: {}", tokens.front()));
    }
  }

  // This function handles statements
  ast::Statement* parse_statement(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions) {
    if (tokens.front() == "return") {
      expect("return", tokens);
      ast::Expression* return_val = parse_expression(tokens, 0, expressions);
      expect(";", tokens);
      ast::Return* ret = new ast::Return(return_val);
      return ret;
    } else if (tokens.front() == "if") {
      tokens.pop_front();
      expect("(", tokens);
      ast::Expression* condition = parse_expression(tokens, 0, expressions);
      expect(")", tokens);
      ast::Statement* body = parse_statement(tokens, expressions);
      ast::If* if_statement;
      if (tokens.front() == "else") {
        tokens.pop_front();
        ast::Statement* else_block = parse_statement(tokens, expressions);
        if_statement = new ast::If(condition, body, else_block);
      } else {
        if_statement = new ast::If(condition, body);
      }
      return if_statement;
    } else if (tokens.front() == "break") {
      tokens.pop_front();
      ast::Identifier* label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
      ast::Break* break_statement = new ast::Break(label);
      expect(";", tokens);
      return break_statement;
    } else if (tokens.front() == "continue") {
      tokens.pop_front();
      ast::Identifier* label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
      ast::Continue* continue_statement = new ast::Continue(label);
      expect(";", tokens);
      return continue_statement;
    } else if (tokens.front() == "while") {
      tokens.pop_front();
      expect("(", tokens);
      ast::Expression* condition = parse_expression(tokens, 0, expressions);
      expect(")", tokens);
      ast::Statement* body = parse_statement(tokens, expressions);
      ast::Identifier* label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
      ast::While* while_statement = new ast::While(condition, body, label);
      return while_statement;
    } else if (tokens.front() == "do") {
      tokens.pop_front();
      ast::Statement* body = parse_statement(tokens, expressions);
      expect("while", tokens);
      expect("(", tokens);
      ast::Expression* condition = parse_expression(tokens, 0, expressions);
      expect(")", tokens);
      expect(";", tokens);
      ast::Identifier* label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
      ast::DoWhile* do_while_statement = new ast::DoWhile(body, condition, label);
      return do_while_statement;
    } else if (tokens.front() == "for") {
      tokens.pop_front();
      expect("(", tokens);
      ast::For_Init* init;
      if (type_aliases.count(tokens.front()) || std::find(specifiers.begin(), specifiers.end(), tokens.front()) != specifiers.end()) {
        std::pair<ast::Type*, ast::Storage_Class> type_and_storage_class = parse_type_and_storage_class(tokens);
        ast::Var_Decl* variable_declaration = dynamic_cast<ast::Var_Decl*>(parse_declaration(tokens, expressions, type_and_storage_class));
        if (!variable_declaration) { throw std::runtime_error("Expected variable declaration in for loop."); }
        init = new ast::Init_Decl(variable_declaration);
      } else {
        if (tokens.front() == ";") {
          tokens.pop_front();
          init = new ast::Init_Exp();
        } else {
          ast::Expression* exp = parse_expression(tokens, 0, expressions);
          init = new ast::Init_Exp(exp);
          expect(";", tokens);
        }
      }

      ast::Expression* condition = nullptr;
      if (tokens.front() != ";") { condition = parse_expression(tokens, 0, expressions); }
      expect(";", tokens);

      ast::Expression* post = nullptr;
      if (tokens.front() != ")") { post = parse_expression(tokens, 0, expressions); }
      expect(")", tokens);
      ast::Statement* body = parse_statement(tokens, expressions);
      ast::Identifier* label = new ast::Identifier("67"); // dummy value since variables cant contian only numbers
      ast::For* for_statement = new ast::For(init, body, label, condition, post);
      return for_statement;
    } else if (tokens.front() == ";") {
      expect(";", tokens);
      ast::Null* null_statement = new ast::Null();
      return null_statement;
    } else if (tokens.front() == "{") {
      expect("{", tokens);
      std::vector<std::unique_ptr<ast::Block_Item>> block_items;
      while (tokens.front() != "}") { block_items.push_back(parse_block_item(tokens, expressions)); }
      ast::Block* block = new ast::Block(std::move(block_items));
      expect("}", tokens);
      ast::Compound_Statement* compound = new ast::Compound_Statement(block);
      return compound;
    } else {
      ast::Expression* exp = parse_expression(tokens, 0, expressions);
      ast::Expression_Statement* exp_statement = new ast::Expression_Statement(exp);
      expect(";", tokens);
      return exp_statement;
    }
    return nullptr;
  }

  ast::Declaration* parse_declaration(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions, std::pair<ast::Type*, ast::Storage_Class>& type_and_storage_class) {
    ast::Identifier* identifier;
    ast::Declaration* declaration;
    char* endptr;
    std::strtol(tokens.front().c_str(), &endptr, 10); // parses decimal integers
    if (std::next(tokens.begin()) == tokens.end()) { throw std::runtime_error("Missing semicolon."); }
    std::string next_token(*std::next(tokens.begin()));
    if (*endptr != '\0' && tokens.front() != "return" && std::regex_match(tokens.front(), tools::naming_convention)) {
      identifier = new ast::Identifier(tokens.front());
      tokens.pop_front();
    } else {
      throw std::runtime_error(std::format("{} is not a valid variable or function name.", next_token));
    }
    if (next_token == "=") {
      expect("=", tokens);
      ast::Expression* exp = parse_expression(tokens, 0, expressions);
      declaration = new ast::Var_Decl(identifier, exp, type_and_storage_class.second);
      expect(";", tokens);
    } else if (next_token == ";") {
      declaration = new ast::Var_Decl(identifier, nullptr, type_and_storage_class.second);
      expect(";", tokens);
    } else if (next_token == "(") // resolve_function will handle whether or not this is allowed to contain a definition
    {
      expect("(", tokens);
      std::vector<std::unique_ptr<ast::Identifier>> param_list;
      parse_parameters(tokens, param_list, identifier->text);
      ast::Block* body = nullptr;
      if (tokens.front() == "{") // Function declaration with a definition
      {
        tokens.pop_front();
        std::vector<std::unique_ptr<ast::Block_Item>> function_body;
        while (tokens.front() != "}") {
          std::unique_ptr<ast::Block_Item> next_block_item = parse_block_item(tokens, expressions);
          function_body.push_back(std::move(next_block_item));
        }
        expect("}", tokens);
        body = new ast::Block(std::move(function_body));
      } else // Basic function declaration without a definition
      {
        expect(";", tokens);
      }
      declaration = new ast::Fun_Decl(identifier, std::move(param_list), std::move(expressions), body, type_and_storage_class.second);
    } else {
      throw std::runtime_error("Not a valid declaration.");
    }
    return declaration;
  }

  std::unique_ptr<ast::Block_Item> parse_block_item(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Expression>>& expressions) {
    // handling typedef
    if (tokens.front() == "typedef") {
      tokens.pop_front();
      if (!type_aliases.count(tokens.front())) { throw std::runtime_error(std::format("{} is not a valid type.", tokens.front())); }

      std::string data_type(tokens.front());
      if (type_aliases.count(data_type)) { // handle nested typedef
        data_type = type_aliases[data_type];
      }
      tokens.pop_front();

      std::string alias(tokens.front());
      tokens.pop_front();
      type_aliases[alias] = data_type;

      std::unique_ptr<ast::S> s = std::make_unique<ast::S>(new ast::Null());
      return s;
    }
    if (type_aliases.count(tokens.front()) || std::find(specifiers.begin(), specifiers.end(), tokens.front()) != specifiers.end()) {
      std::pair<ast::Type*, ast::Storage_Class> type_and_storage_class = parse_type_and_storage_class(tokens);
      std::unique_ptr<ast::D> d = std::make_unique<ast::D>(parse_declaration(tokens, expressions, type_and_storage_class));
      // delete declaration;
      return d;
    } else {
      ast::Statement* unresolved_statement = parse_statement(tokens, expressions);
      std::unique_ptr<ast::S> s = std::make_unique<ast::S>(unresolved_statement);
      return s;
    }
  }

  void parse_parameters(std::list<std::string>& tokens, std::vector<std::unique_ptr<ast::Identifier>>& params, const std::string& func_name) {
    while (!tokens.empty() && tokens.front() != ")") {
      if (type_aliases.count(tokens.front()) || "int" == tokens.front()) {
        tokens.pop_front();
        params.push_back(std::make_unique<ast::Identifier>(tokens.front()));
        tokens.pop_front();
        if (tokens.front() == ")") break;
        else {
          expect(",", tokens);
          if (!tokens.empty() && !(type_aliases.count(tokens.front()) || tokens.front() == "int")) {
            throw std::runtime_error(std::format("Trailing comma in parameter list of function {}.", func_name));
          }
        }
      } else if (tokens.front() == "void") {
        tokens.pop_front();
        if (!params.empty()) { throw std::runtime_error(std::format("More than one parameter in function {} declared with a void parameter.", func_name)); }
        if (!tokens.empty() && tokens.front() != ")") { throw std::runtime_error(std::format("Expected * after void in parameter list of function {}.", func_name)); }
        break;
      } else {
        throw std::runtime_error(std::format("{} should not be in the parameter list of function {}.", tokens.front(), func_name));
      }
    }
    expect(")", tokens);
  }

  // this function is a bit different from the textbook, but it allows us to handle the entire parsing process for specifiers in one function
  std::pair<ast::Type*, ast::Storage_Class> parse_type_and_storage_class(std::list<std::string>& tokens) {
    std::vector<std::string> specifier_list;
    while (!tokens.empty() && (type_aliases.count(tokens.front()) || std::find(specifiers.begin(), specifiers.end(), tokens.front()) != specifiers.end())) {
      specifier_list.push_back(tokens.front());
      tokens.pop_front();
    }
    if (tokens.empty()) throw std::runtime_error("Expected identifier after type specifiers, got end of file.");

    std::vector<std::string> types;
    std::vector<std::string> storage_classes;
    for (const std::string& specifier : specifier_list) {
      if (type_aliases.count(specifier) || specifier == "int") {
        types.push_back(specifier);
      } else {
        storage_classes.push_back(specifier);
      }
    }

    if (types.size() != 1) throw std::runtime_error("Invalid Type Specifier");
    if (storage_classes.size() > 1) throw std::runtime_error("Invalid Storage Class");

    ast::Int* type = nullptr; // for now, this is just a placeholder
    ast::Storage_Class storage_class;

    if (storage_classes.size() == 1) {
      storage_class = ast::get_storage_class(storage_classes[0]);
    } else {
      storage_class = ast::Storage_Class::NONE;
    }

    return std::make_pair(type, storage_class); // nothing uses type for now so this is okay
  }

  // Handles file scope declarations
  ast::Program* parse_program(std::list<std::string>& tokens) {
    std::vector<std::unique_ptr<ast::Declaration>> declarations;
    std::vector<std::unique_ptr<ast::Expression>> global_expressions; // this is used to store variable declarations that are not part of a function declaration
    while (!tokens.empty()) {
      if (!(type_aliases.count(tokens.front()) || std::find(specifiers.begin(), specifiers.end(), tokens.front()) != specifiers.end())) {
        throw std::runtime_error(std::format("Expected variable or function declaration, got {}.", tokens.front()));
      }
      std::pair<ast::Type*, ast::Storage_Class> type_and_storage_class = parse_type_and_storage_class(tokens);

      std::vector<std::unique_ptr<ast::Expression>> expressions; // doesnt matter if this is empty after move since global variables don't own expressions anyways
      ast::Declaration* declaration = parse_declaration(tokens, expressions, type_and_storage_class);

      if (dynamic_cast<ast::Var_Decl*>(declaration)) {
        for (auto& expression : expressions) {
          global_expressions.push_back(std::move(expression)); // move global expressions to the global_expressions vector
        }
      }

      declarations.emplace_back(declaration);
    }
    if (!tokens.empty()) { throw std::runtime_error("Extra Characters Found For Minimal Compiler."); }

    type_aliases.clear();
    return new ast::Program(std::move(declarations), std::move(global_expressions));
  }

  // This function converts Tokens into AST Program node
  ast::Program* parse(std::list<std::string>& tokens) { return parse_program(tokens); }
} // namespace parser
