# Makefile to run the compiler under x86_64 architecture

.PHONY: 86_term check_term macos_setup clone_tests clean compile debug compile_13 debug_13

STAGE ?= full

LEX_SRCS      = src/driver.cpp src/lexer.cpp
PARSE_SRCS    = $(LEX_SRCS) src/parser.cpp src/tools.cpp
VALIDATE_SRCS = $(PARSE_SRCS) src/semantic_analysis.cpp
TACKY_SRCS    = $(VALIDATE_SRCS) src/ir_gen.cpp
CODEGEN_SRCS  = $(TACKY_SRCS) src/codegen.cpp 
FULL_SRCS     = $(CODEGEN_SRCS) src/aast.cpp

ifeq ($(STAGE),lex)
  SRCS := $(LEX_SRCS)
  STAGE_FLAG := -DCOMPILER_STAGE=1
else ifeq ($(STAGE),parse)
  SRCS := $(PARSE_SRCS)
  STAGE_FLAG := -DCOMPILER_STAGE=2
else ifeq ($(STAGE),validate)
  SRCS := $(VALIDATE_SRCS)
  STAGE_FLAG := -DCOMPILER_STAGE=3
else ifeq ($(STAGE),tacky)
  SRCS := $(TACKY_SRCS)
  STAGE_FLAG := -DCOMPILER_STAGE=4
else ifeq ($(STAGE),codegen)
  SRCS := $(CODEGEN_SRCS)
  STAGE_FLAG := -DCOMPILER_STAGE=5
else ifeq ($(STAGE),full)
  SRCS := $(FULL_SRCS)
  STAGE_FLAG := -DCOMPILER_STAGE=6
else
  $(error Invalid STAGE='$(STAGE)'. Use lex, parse, validate, tacky, codegen, or full)
endif

CXXFLAGS = -std=c++20 -Iinclude $(STAGE_FLAG) -Wall -Wextra -g

86_term:
	@arch -x86_64 /bin/zsh

check_term:
	@uname -m

macos_setup:
	@brew install python
	@softwareupdate --install-rosetta --agree-to-license
	@mkdir -p bin

clone_tests:
	@git clone https://github.com/nlsandler/writing-a-c-compiler-tests.git

clean:
	@rm -rf bin assembly* preprocessed*

compile:
	@mkdir -p bin
	@g++ $(CXXFLAGS) $(SRCS) -o bin/zwcc

compile_13:
	@mkdir -p bin
	@g++-13 $(CXXFLAGS) $(SRCS) -o bin/zwcc

debug:
	@mkdir -p bin
	@g++ $(CXXFLAGS) -DDEBUG $(SRCS) -o bin/zwcc

debug_13:
	@mkdir -p bin
	@g++-13 $(CXXFLAGS) -DDEBUG $(SRCS) -o bin/zwcc
