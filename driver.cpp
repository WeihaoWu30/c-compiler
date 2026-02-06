#include <cstdlib>
#include <iostream>
#include <format>
#include <cunistd.h>

void main(int argc, char* argv[]){
  if (argc == 1) exit(1);

  char* cmds[][] = {
    {"gcc", "-E", "-P", argv[1], "-o", "preprocessed.i", nullptr},
    {/*compile*/},
    {"gcc assembly.s -o a.out", nullptr}
  };
  pid_t pid = fork();

  for (int i = 0; i < 4; i++){
    if (pid == 0){
      execvp(args[i][0], args[i]);
      perror("error");
      exit(1);
    }
    else if (pid > 0){
      wait(nullptr);
    }
  }

  //Compile the preproccesd source file and output an assembly file...delete preprocessed file after

  //Assemble and link the assmebly file to produce an executable
       
  exit(0);
}
