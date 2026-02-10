#include "lexer.h"
#include "parser.h"
#include <cstring>
#include <list>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#define PREPROCESSED_FILE "preprocessed.i"

void preprocess(char *filename) {
  if (!fork()) {
    execlp("gcc", "gcc", "-E", "-P", filename, "-o", PREPROCESSED_FILE,
           (char *)nullptr);
  } else {
    wait(nullptr);
  }
}

void cleanup() {
  if (!fork()) {
    execlp("rm", "rm", PREPROCESSED_FILE, (char *)nullptr);
    // execlp("gcc", "-S", "-O", "-fno-asynchronous-unwind-tables",
    // "-fcf-protection=none", filename);
    _exit(1);
  }
  wait(nullptr);
}

void assemble(char *filename) {
  if (!fork()) {
    execlp("gcc", filename);
  } else {
    wait(nullptr);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return 1;
  }
  std::list<std::string> tokens;

  preprocess(argv[1]);
  if (argc > 2) {
    std::string s(argv[2]);
    if (s.compare("--lex") == 0) {
      lex(PREPROCESSED_FILE);
    } else if (s.compare("--parse") == 0) {
      tokens = lex(PREPROCESSED_FILE);
      parse(tokens);
    } else if (s.compare("--codegen") == 0) {
      tokens = lex(PREPROCESSED_FILE);
      parse(tokens);
    }
  } else {
    tokens = lex(PREPROCESSED_FILE);
    parse(tokens);
    // codegen();
  }
  // cleanup();
  // assemble("preprocessed.s");

  return 0;
}
