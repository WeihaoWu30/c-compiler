#include <cstdlib>
#include <iostream>
#include <format>
#include <unistd.h>
#include <string>
#include <regex>
#include <list>
#include <fstream>

int main(int argc, char* argv[]){
  std::vector<std::regex> patterns = {
    std::regex ("[a-zA-Z_]\\w*\\b"),
    std::regex ("[0-9]+\\b"),
    std::regex ("int\\b"),
    std::regex ("void\\b"),
    std::regex ("return\\b"),
    std::regex ("\\("),
    std::regex ("\\)"),
    std::regex ("\\{"),
    std::regex ("\\}"),
    std::regex (";")};
			
		      
  std::string line;
  std::smatch matches;
  std::list<std::string> tokens;
  std::ifstream input_file(argv[1]);

  while (std::getline(input_file, line)){
    while (line[0] == ' '){
      line = line.substr(1);
    }
    while(line.size() > 0){
      for (unsigned int i = 0; i < patterns.size(); i++){
	if (std::regex_search(line, matches, patterns[i])){
	  std::string::size_type pos = matches.position(0);
	  if (pos == 0){
	    std::string::size_type len = matches.length(0);
	    line = line.substr(len);
	  }
	}
      }
    }
  }
}
