# Compiler Driver

## If Arch command does not work:
softwareupdate --install-rosetta --agree-to-license

## Arch Command:
arch x86_64 /bin/zsh

## Check Shell:
uname -m

## C-compiler tests repo:
https://github.com/nlsandler/writing-a-c-compiler-tests.git

## Assembly File Compilation
gcc -S -O -fno-asynchronous-unwind-tables -fcf-protection=none <filename>

## Preprocessor Compilation
gcc -E -P <input_file> -o <preprocessed_file>

## Compile Compiler
### Make a bin folder if you haven't done so already
mkdir bin
### Compile With C++
g++ -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g

## Compiler Options
- ./bin/zwcc --lex <filename>
- ./bin/zwcc --parse <filename>
- ./bin/zwcc --tacky <filename>
- ./bin/zwcc --codegen <filename>
- ./bin/zwcc <filename>

