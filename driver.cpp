#include <cstring>
#include <sys/wait.h>
#include <unistd.h>

#define PREPROCESSED_FILE "preprocessed.i"

void preprocess(char *filename) {
  if (!fork()) {
    execlp("gcc", "gcc", "-E", "-P", filename, "-o", "preprocessed.i",
           (char *)nullptr);
  } else {
    wait(nullptr);
  }
}

// Compile Lexer
void compile(char *filename) {
  if (!fork()) {
    execlp("g++", "g++", "lexer.cpp", "-o", "lexer", (char *)nullptr);
    // execlp("gcc", "-S", "-O", "-fno-asynchronous-unwind-tables",
    // "-fcf-protection=none", filename);
    _exit(1);
  }
  wait(nullptr);

  if (!fork()) {
    execl("./lexer", "./lexer", filename, (char *)nullptr);
    _exit(1);
  }
  wait(nullptr);
}

void cleanup() {
  if (!fork()) {
    execlp("rm", "rm", PREPROCESSED_FILE, (char *)nullptr);
    // execlp("gcc", "-S", "-O", "-fno-asynchronous-unwind-tables",
    // "-fcf-protection=none", filename);
    _exit(1);
  }
  wait(nullptr);

  if (!fork()) {
    execlp("rm", "rm", "lexer", (char *)nullptr);
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
  if (argc == 1)
    return 1;

  preprocess(argv[1]);
  compile(PREPROCESSED_FILE);
  cleanup();
  // assemble("preprocessed.s");

  // Compile the preproccesd source file and output an assembly file...delete
  // preprocessed file after

  // Assemble and link the assmebly file to produce an executable

  return 0;
}
