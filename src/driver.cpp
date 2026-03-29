#include "aast/top_level/program.hpp"
#include "ast/top_level/program.hpp"
#include "tacky/top_level/program.hpp"
#include "compiler/compiler.hpp"
#include <cstring>
#include <fstream>
#include <list>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <memory>

// Intermediate Files that will be deleted
#define PPF "preprocessed.i"
#define AF "assembly.s"

// Prototyping
void preprocess(char *filename);
void cleanup();
void execute(const char *filename);
int main(int argc, char *argv[]);

/*
This function calls the GCC driver to remove comments and trims whitespace in the C file provided.
This makes it easier for the compiler to parse and lex the C file.
*/
void preprocess(char *filename)
{
  if (!fork())
  {
    execlp("gcc", "gcc", "-E", "-P", filename, "-o", PPF, (char *)nullptr);
    _exit(1);
  }
  else
  {
    wait(nullptr);
  }
}

// This function removes the intermediate files
void cleanup()
{
  if (!fork())
  {
    execlp("rm", "rm", PPF, AF, (char *)nullptr);
    _exit(1);
  }
  else
  {
    wait(nullptr);
  }
}

// This function converts Assembly File To Binary Executable
void execute(const char *filename)
{
  if (!fork())
  {
    execlp("gcc", "gcc", AF, "-o", filename, (char *)nullptr);
    _exit(1);
  }
  else
  {
    wait(nullptr);
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    return 1;
  }
  std::list<std::string> tokens;
  std::unique_ptr<ast::Program> program;
  std::unique_ptr<tacky::Program> tacky_program;
  std::unique_ptr<aast::Program> assembly_program;
  if (argc == 3)
  { // when a flag is specified to the compiler
    preprocess(argv[2]);
    tokens = lexer::lex(PPF);
    std::string s(argv[1]); // this is a must since temporary strings are strings and not const chars
    if (s.compare("--lex") == 0)
      return 0;
    if (s.compare("--parse") == 0)
    {
      program = parser::parse(tokens);
    }
    else if (s.compare("--validate") == 0)
    {
      program = parser::parse(tokens);
    }
    else if (s.compare("--tacky") == 0)
    {
      program = parser::parse(tokens);
      tacky_program = ir_gen::generate_tacky(std::move(program));
    }
    else if (s.compare("--codegen") == 0)
    {
      program = parser::parse(tokens);
      tacky_program = ir_gen::generate_tacky(std::move(program));
      assembly_program = codegen::generate_top_level(std::move(tacky_program));
    }
  }
  else
  {
    preprocess(argv[1]);
    tokens = lexer::lex(PPF);
    program = parser::parse(tokens);
    //  tacky_program = ir_gen::generate_tacky(program);
    assembly_program = codegen::generate_top_level(std::move(tacky_program));
    std::ofstream ostr(AF);
    if (!ostr)
    {
      std::cerr << "Failed To Open Assembly File" << std::endl;
      return 1;
    }

    ostr << *(assembly_program); // Writing Assembly To File using Output Stream Extraction Operator Overloading
    ostr.close();

    // This is just for the test cases to pass
    std::string s(argv[1]);
    s.erase(s.size() - 2);
    execute(s.c_str());
    cleanup();
  }
  return 0;
}
