#include <cstdlib>
#include <iostream>
#include <format>
#include <unistd.h>

struct Command{
   char* preprocess[7];
   char* compile[1];
   char* assemble[5];
};

void main(int argc, char* argv[]){
  if (argc == 1) exit(1);



  Command cmds = {
    {"gcc", "-E", "-P", argv[1], "-o", "preprocessed.i", nullptr},
    {/*compile*/},
    {"gcc",  "assembly.s", "-o", "a.out", nullptr}
  };

  for (int i = 0; i < 4; i++){
    pid_t pid = fork();
    if (pid == 0){
      if (i == 0){
         execvp(cmds.preprocess[0], cmds.preprocess);
      }
      else if (i == 1){
         //compile
      }
      else if (i == 2){
         execvp(cmds.assemble[0], cmds.assemble);
      }
    }
    else if (pid > 0){
      wait(nullptr);
    }
  }

  //Compile the preproccesd source file and output an assembly file...delete preprocessed file after

  //Assemble and link the assmebly file to produce an executable
       
  exit(0);
}
