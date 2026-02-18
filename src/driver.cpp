#include "aast.h"
#include "ast.h"
#include "tacky.h"
#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "ir_gen.h"
#include <cstring>
#include <fstream>
#include <list>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#define PPF "preprocessed.i"
#define AF "assembly.s"

void preprocess(char *filename) {
  if (!fork()) {
    execlp("gcc", "gcc", "-E", "-P", filename, "-o", PPF, (char *)nullptr);
    _exit(1);
  } else {
    wait(nullptr);
  }
}

void cleanup(Program *&program, AProgram *&assembly_program, TProgram *&tacky_program) {
  delete program;
  delete assembly_program;
  delete tacky_program;
  if (!fork()) {
    execlp("rm", "rm", PPF, AF, (char *)nullptr);
    _exit(1);
  }
  wait(nullptr);
}

void execute(const char *filename) {
	std::cout << filename << std::endl;
  if (!fork()) {
    execlp("gcc", "gcc", AF, "-o", filename, (char *)nullptr);
    _exit(1);
  } else {
    wait(nullptr);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return 1;
  }
  std::list<std::string> tokens;
  Program *program = nullptr;
  AProgram *assembly_program = nullptr;
  TProgram *tacky_program = nullptr;
  if (argc == 3) {
    preprocess(argv[2]);
    tokens = lex(PPF);
    std::string s(argv[1]);
    if (s.compare("--lex") == 0) return 0;
    if (s.compare("--parse") == 0) {
      program = parse(tokens);
    } else if (s.compare("--tacky") == 0){
      program = parse(tokens);
      tacky_program = generate_tacky(program);
    } else if (s.compare("--codegen") == 0) {
      program = parse(tokens);
      tacky_program = generate_tacky(program);
      assembly_program = codegen(program);
    }
  } else {
    preprocess(argv[1]);
    tokens = lex(PPF);
    program = parse(tokens);
    assembly_program = codegen(program);
    std::ofstream ostr(AF);
    if (!ostr) {
      std::cerr << "Failed To Open Assembly File" << std::endl;
      return 1;
    }

    ostr << *(assembly_program);
    ostr.close();

    std::string s(argv[1]);
    s.erase(s.size() - 2);
    execute(s.c_str());
  }

  cleanup(program, assembly_program, tacky_program);
  return 0;
}
