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


