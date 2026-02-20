#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <string>
#include <unistd.h>

std::list<std::string> lex(const std::string &filename) {
  std::vector<std::regex> patterns = {std::regex("int\\b"),
                                      std::regex("void\\b"),
                                      std::regex("return\\b"),
                                      std::regex("[0-9]+\\b"),
                                      std::regex("[a-zA-Z_]\\w*\\b"),
                                      std::regex("\\("),
                                      std::regex("\\)"),
                                      std::regex("\\{"),
                                      std::regex("\\}"),
                                      std::regex(";"),
                                      std::regex("--"),
                                      std::regex("-"),
                                      std::regex("~")};

  std::string line;
  std::smatch matches;
  std::list<std::string> tokens;
  std::ifstream istr(filename);

  while (istr >> line) {
    while (line.size() > 0) {
      bool found = false; // Flag To Detect Illegal Expression
      std::string longestMatch;
      long len = 0; // Length of Longest Pattern Match
      for (unsigned int i = 0; i < patterns.size(); ++i) {
        if (std::regex_search(line, matches, patterns[i])) {
          long pos = matches.position(0);
          if (pos == 0 && matches.length(pos) > len) { // Pattern needs to match starting from the first character to be valid
            longestMatch = matches.str(pos);
            len = matches.length(pos);
            found = true;
          }
        }
      }
      if (!found) {
        std::cerr << "Illegal Expression: " << line << std::endl;
        exit(1);
      } else {
        tokens.push_back(longestMatch);
        line = line.substr(len);
      }
    }
  }

  return tokens;
}
