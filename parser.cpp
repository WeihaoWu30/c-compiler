#include <iostream>
#include <vector>
#include <string>


class Program{
   Function function;
};

class Function : public Program{
   public:
      Function();
   private:
      Identifier name;
      Statement body;
};

class Expression;

class Statement: public Function{
   public:
      Statement();
   private:
      Expression exp;
};


class Expression: public Statement{
   public:
      Expression();
      Expression(Constant val): value(val) {}
      Constant get_value(){ return value; }
   
   private:
   Constant value;
};

class Constant: public Expression{   //constant tokens
   public:
      Constant(std::string val): value(val) {}
   private:
      std::string value;
};

class Identifier: public Function{    //identifer tokens

};

class Return: public Expression{
   public:
      Return(Expression exp): expr(exp) {}
   private:
      Expression expr;
};

std::string take_token(std::vector<std::string> tokens){
   return tokens[0];
}

bool expect(std::string expected, std::vector<std::string> tokens){
   std::string actual = take_token(tokens);
   if (actual == expected){
      tokens.erase(tokens.begin());
      return true;
   }
   else{
      std::cerr << "Syntax Error" << std::endl;
   }
}

Constant parse_expression(std::vector<std::string> tokens){
   Expression exp(tokens[0]);
   return exp.get_value();
}

int parse_function(std::vector<std::string> tokens){

}

int parse_statement(std::vector<std::string> tokens){
   expect("return", tokens);
   parse_expression(tokens);
   expect(";", tokens);
}






int main() {
  std::cout << "Hello, World!" << std::endl;
  return 0;
}