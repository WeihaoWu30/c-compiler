#ifndef IDENTIFIER_H
#define IDENTIFIER_H
#include <string>

class Identifier {
public:
   Identifier(std::string name_): name(name_) {}
   friend std::ostream& operator<<(std::ostream& ostr, const Identifier& identifier);
private:
   std::string name;
};

#endif