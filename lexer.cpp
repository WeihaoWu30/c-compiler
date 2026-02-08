#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unistd.h>
#include <vector>

void trim(std::string &s) {
  s.erase(s.begin(), std::find_if_not(s.begin(), s.end(),
                                      [](const char &c) { return c == ' '; }));
}
int main(int argc, char *argv[]) {
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
  std::vector<std::string> tokens;
  std::ifstream istr(argv[1]);

  while (istr >> line) {
    while (line.size() > 0) {
      bool found = false;
      for (unsigned int i = 0; i < patterns.size(); ++i) {
        if (std::regex_search(line, matches, patterns[i])) {
          long pos = matches.position(0);
          if (pos == 0) {
            line = line.substr(matches.length(pos));
            tokens.push_back(matches.str(pos));
            found = true;
            break;
          }
        }
      }
      if (!found) {
        std::cerr << "No Regex Pattern Found For " << line << std::endl;
        return 1;
      }
    }
  }

  return 0;
}
