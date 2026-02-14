#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <string>
#include <unistd.h>

void trim(std::string &s) {
  s.erase(s.begin(), std::find_if_not(s.begin(), s.end(),
                                      [](const char &c) { return c == ' '; }));
}
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
                                      std::regex(";")};

  std::string line;
  std::smatch matches;
  std::list<std::string> tokens;
  std::ifstream istr(filename);

  while (istr >> line) {
    while (line.size() > 0) {
      bool found = false;
      for (unsigned int i = 0; i < patterns.size(); ++i) {
        if (std::regex_search(line, matches, patterns[i])) {
          long pos = matches.position(0);
          if (pos == 0) {
            tokens.push_back(matches.str(pos));
            line = line.substr(matches.length(pos));
            found = true;
            break;
          }
        }
      }
      if (!found) {
        std::cerr << "Illegal Expression: " << line << std::endl;
        exit(1);
      }
    }
  }

  return tokens;
}
