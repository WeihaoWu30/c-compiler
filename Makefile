# Makefile to run the compiler under x86_64 architecture

.PHONY: term setup clean compile run

term:
	@arch -x86_64 /bin/zsh

setup:
	@brew install python
	@softwareupdate --install-rosetta --agree-to-license

clean:
	@rm -rf bin

compile: 
	@g++ -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g



