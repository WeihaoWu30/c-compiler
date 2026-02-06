#include <cstdlib>
#include <iostream>
#include <format>
#include <unistd.h>
#include <string>
#include <regex>

int main(int argc, char* arv[]){
  std::vector<std::regex> patterns = {
    std::regex identifier("[a-zA-Z_]\w*\b"),
    std::regex constant("[0-9]+\b"),
    std::regex int_key("int\b"),
    std::regex void_key("void\b"),
    std::regex return_key("return\b"),
    std::regex open_paren("\("),
    std::regex close_paren("\)"),
    std::regex open_brace("\{"),
    std::regex close_brace("\}"),
    std::regex semicolon(";")};
			
		      
  std::string line;
  std::smatch matches;
  std::list<std::string> tokens;
  
  while (std::get_line(argv[1], line)){
    while (line[0] == " "){
      line = line.substr(1);
    }
    while(line.size() > 0){
      for (usigned int i = 0; i < patterns.size(); i++){
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
