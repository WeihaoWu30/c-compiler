# Makefile to run the compiler under x86_64 architecture

.PHONY: term setup clean compile debug compile_13 debug_13

term:
	@arch -x86_64 /bin/zsh

setup:
	@brew install python
	@softwareupdate --install-rosetta --agree-to-license
	@mkdir -p bin

clean:
	@rm -rf bin

compile: 
	@g++ -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g

compile_13:
	@g++-13 -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g

debug:
	@g++ -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g -DDEBUG

debug_13:
	@g++-13 -std=c++20 -Iinclude src/*.cpp -o bin/zwcc -Wall -Wextra -g -DDEBUG



