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
#include <stdexcept>
#include <cstdlib>
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
  try
  {
    if (argc < 2)
    {
      throw std::runtime_error("Missing Filename.");
    }
    std::unique_ptr<ast::Program> program;
    std::unique_ptr<aast::Program> assembly_program;
    std::unique_ptr<tacky::Program> tacky_program;
    if (argc == 3)
    { // when a flag is specified to the compiler
      preprocess(argv[2]);
      std::list<std::string> tokens = lexer::lex(PPF);
      std::string s(argv[1]); // this is a must since temporary strings are strings and not const chars
      if (s.compare("--lex") == 0)
        return 0;
      if (s.compare("--parse") == 0)
      {
        program.reset(parser::parse(tokens)); // unique ptr owns raw ptr
      }
      else if (s.compare("--tacky") == 0)
      {
        program.reset(parser::parse(tokens));
        tacky_program.reset(ir_gen::generate_tacky(program.get()));
      }
      else if (s.compare("--codegen") == 0)
      {
        program.reset(parser::parse(tokens));
        tacky_program.reset(ir_gen::generate_tacky(program.get()));
        assembly_program.reset(codegen::generate_top_level(tacky_program.get()));
      }
      else
      {
        throw std::runtime_error("Unknown flag: " + s);
      }
    }
    else
    {
      preprocess(argv[1]);
      std::list<std::string> tokens = lexer::lex(PPF);
      program.reset(parser::parse(tokens));
      tacky_program.reset(ir_gen::generate_tacky(program.get()));
      assembly_program.reset(codegen::generate_top_level(tacky_program.get()));
      std::ofstream ostr(AF);
      if (!ostr)
      {
        throw std::runtime_error("Failed To Open Assembly File");
      }

      ostr << *assembly_program; // Writing Assembly To File using Output Stream Extraction Operator Overloading
      ostr.close();

      // This is just for the test cases to pass
      std::string s(argv[1]);
      s.erase(s.size() - 2);
      execute(s.c_str());

#ifndef DEBUG
      cleanup();
#endif
    }
    return EXIT_SUCCESS;
  }
  catch (const std::exception &e)
  {
#ifndef DEBUG
    cleanup();
#endif
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
