#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#include "lexer.h"
#include "parser.h"

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
    execlp("rm", "rm", PREPROCESSED_FILE, "lexer", (char *)nullptr);
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
  if (argc < 2) return 1;
  std::list<std::string> tokens;
  
  preprocess(argv[1]);
  if(argc > 2) {
    switch(argv[2]){
      case "--lex":
        lex(PREPROCESSED_FILE);
        break;
      case "--parse":
        tokens = lex(PREPROCESSED_FILE);
        parse(tokens);
        break;
      case "--codegen":
        tokens = lex(PREPROCESSED_FILE);
        parse(tokens);
        //codegen();
        break;
  } else {
    tokens = lex(PREPROCESSED_FILE);
    parse(tokens);
    // codegen();
  }
  cleanup();
  // assemble("preprocessed.s");

  return 0;
}
