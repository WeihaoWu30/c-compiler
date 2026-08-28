# ZWCC

ZWCC is a multi-pass C compiler written in C++20. It compiles a substantial subset of C into x86-64 assembly and produces native executables for x86-64 Linux and macOS.

The compiler implements the complete frontend and backend pipeline, including lexical analysis, recursive-descent parsing, semantic validation, intermediate representation generation, instruction lowering, assembly emission, and executable creation.

> [!NOTE]
> ZWCC is an educational compiler under active development. It does not yet implement the complete C standard.

## Compiler Pipeline

```text
C source
   ↓
Preprocessor
   ↓
Lexer and tokens
   ↓
Abstract syntax tree
   ↓
Semantic analysis
   ↓
TACKY intermediate representation
   ↓
Assembly AST
   ↓
x86-64 assembly
   ↓
Object file
   ↓
Executable
```

Intermediate preprocessed, assembly, and object files are removed after the executable is created unless compilation is stopped at an earlier stage.

## Features

ZWCC currently supports:

* Integer constants and variables
* Unary, arithmetic, logical, relational, and bitwise operators
* Simple and compound assignment
* Operator precedence and associativity
* Lexical scoping and variable resolution
* Conditional expressions and `if` statements
* Compound statements
* `while`, `do-while`, and `for` loops
* `break` and `continue`
* Function declarations, definitions, and calls
* Register and stack argument passing
* File-scope variables
* Static storage duration
* `static` and `extern` storage-class specifiers
* Compile-time semantic error detection
* x86-64 code generation for Linux and macOS

Function calls follow the System V AMD64 ABI, including register and stack argument passing and 16-byte stack alignment.

## Architecture

The compiler is divided into modular passes so each representation can be validated and transformed independently.

Major implementation details include:

* Recursive-descent parsing into a typed abstract syntax tree
* Semantic passes for identifier resolution, label validation, and type-related checks
* TACKY, a three-address intermediate representation
* Separate AST, IR, and assembly node representations
* C++ variants and visitors for node processing
* Smart pointers and arena-style containers for object lifetime management
* Instruction fixup and stack allocation passes
* Platform-specific symbol emission for Linux and macOS
* Automated preprocessing, assembly, linking, and intermediate-file cleanup

## Requirements

* A C++20-compatible compiler
* GCC or Clang
* Make
* Python 3.8 or newer for the official test suite
* An x86-64 Linux or macOS environment

## Building

Clone the repository and create the output directory:

```bash
git clone <your-repository-url>
cd <repository-name>
mkdir -p bin
```

Compile ZWCC:

```bash
g++ -std=c++20 -Iinclude src/*.cpp \
    -o bin/zwcc \
    -Wall -Wextra -g
```

If using the included Makefile:

```bash
make
```

The resulting compiler executable is located at:

```text
bin/zwcc
```

## Usage

Compile a C source file into an executable:

```bash
./bin/zwcc example.c
```

The compiler supports flags for stopping after individual stages:

| Command                           | Stage                          |
| --------------------------------- | ------------------------------ |
| `./bin/zwcc --lex example.c`      | Run lexical analysis           |
| `./bin/zwcc --parse example.c`    | Build the abstract syntax tree |
| `./bin/zwcc --validate example.c` | Run semantic validation        |
| `./bin/zwcc --tacky example.c`    | Generate TACKY IR              |
| `./bin/zwcc --codegen example.c`  | Generate assembly AST          |
| `./bin/zwcc -c example.c`         | Compile without linking        |
| `./bin/zwcc example.c`            | Produce a native executable    |

Example:

```c
int factorial(int n) {
    int result = 1;

    for (int i = 2; i <= n; i += 1) {
        result *= i;
    }

    return result;
}

int main(void) {
    return factorial(5);
}
```

Compile and run it:

```bash
./bin/zwcc factorial.c
./factorial
echo $?
```

The program should exit with status code `120`.

## Testing

ZWCC is validated with Nora Sandler’s official companion test suite:

```bash
git clone https://github.com/nlsandler/writing-a-c-compiler-tests.git
cd writing-a-c-compiler-tests
./test_compiler --check-setup
```

Run the tests through Chapter 10, including the implemented bitwise and compound-assignment extensions:

```bash
./test_compiler /path/to/zwcc/bin/zwcc \
    --chapter 10 \
    --bitwise \
    --compound
```

The current implementation passes all **357/357 supported tests**, covering valid programs, invalid-program rejection, semantic analysis, and executable behavior.

The official runner considers a valid test successful when the compiler produces an executable with the expected output and exit code. Invalid tests pass when the compiler correctly rejects the program without producing output files.

## macOS on Apple Silicon

ZWCC currently emits x86-64 assembly rather than native ARM64 assembly. On an Apple Silicon Mac, install Rosetta 2 if it is not already available:

```bash
softwareupdate --install-rosetta --agree-to-license
```

Start an x86-64 shell:

```bash
arch -x86_64 /bin/zsh
```

Confirm the active architecture:

```bash
uname -m
```

The expected result is:

```text
x86_64
```

Then build and run the compiler from that shell.

## Inspecting Compiler Output

Generate assembly from a C source file using the system compiler for comparison:

```bash
gcc -S -O0 \
    -fno-asynchronous-unwind-tables \
    -fcf-protection=none \
    example.c
```

View preprocessor output:

```bash
gcc -E -P example.c -o example.i
```

These commands are useful for comparing ZWCC’s output with GCC or Clang and debugging platform-specific assembly behavior.

## Current Limitations

ZWCC currently targets an integer-based subset of C. It does not yet support:

* Floating-point types
* Pointer and array operations
* Structures and unions
* Character and string literals
* Dynamic memory allocation as a language-level feature
* Native ARM64 code generation
* Optimization passes
* Full register allocation
* The complete ISO C grammar and type system

Some optional language features, including prefix/postfix increment and decrement, `goto`, and `switch`, are not currently implemented.

## Roadmap

Planned work includes:

* Additional integer types and conversions
* Unsigned arithmetic
* Floating-point support
* Pointers and arrays
* Character and string support
* Structures
* Optimization passes
* Register allocation
* Additional test automation
* Native ARM64 code generation

## Attribution

ZWCC was developed while working through Nora Sandler’s [*Writing a C Compiler*](https://nostarch.com/writing-c-compiler). The book presents the compiler algorithms in pseudocode; this project implements the pipeline independently in C++ with its own data structures, compiler passes, driver, and platform-handling logic.

Correctness is evaluated using Sandler’s official [`writing-a-c-compiler-tests`](https://github.com/nlsandler/writing-a-c-compiler-tests) companion test suite.

This repository is an independent educational project and is not affiliated with or endorsed by Nora Sandler or No Starch Press.
