#include <cstring>
#include <sys/wait.h>
#include <unistd.h>

void preprocess(char *filename) {
  if (!fork()) {
    execl("gcc", "-E", "-P", filename, "-o", "preprocessed.i");
  } else {
    wait(nullptr);
  }
}

// change this
void compile(char *filename) {
  if (!fork()) {
    execl("gcc", "-E", "-P", filename, "-o", "preprocessed.i");
  } else {
    wait(nullptr);
  }
}

void assemble() {
  if (!fork()) {
    execl("gcc", "assembly.s");
  } else {
    wait(nullptr);
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1)
    return 1;

  preprocess(argv[1]);
  // compile();
  assemble();

  // Compile the preproccesd source file and output an assembly file...delete
  // preprocessed file after

  // Assemble and link the assmebly file to produce an executable

  return 0;
}
