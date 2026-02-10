#ifndef __IDENTIFIER_H__
#define __IDENTIFIER_H__
#include <string>
#include <iostream>

class Identifier {
public:
   Identifier(std::string name_): name(name_) {}
   friend std::ostream& operator<<(std::ostream& ostr, const Identifier& identifier);
private:
   std::string name;
};

#endif
