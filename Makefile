# Makefile to run the compiler under x86_64 architecture

.PHONY: term macos_setup clone_tests clean compile debug compile_13 debug_13

term:
	@arch -x86_64 /bin/zsh

macos_setup:
	@brew install python
	@softwareupdate --install-rosetta --agree-to-license
	@mkdir -p bin

clone_tests:
	@git clone https://github.com/nlsandler/writing-a-c-compiler-tests.git

clean:
	@rm -rf bin assembly* preprocessed*

compile: 
	@g++ -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g

compile_13:
	@g++-13 -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g

debug:
	@g++ -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g -DDEBUG

debug_13:
	@g++-13 -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g -DDEBUG



