#ifndef __STATEMENT_H__
#define __STATEMENT_H__

#include <ostream>
class Statement {
public:
  virtual ~Statement() = default;

protected:
  virtual void print(std::ostream &ostr) const = 0;
  friend std::ostream &operator<<(std::ostream &ostr, const Statement &ret) {
  	ret.print(ostr);
  	return ostr;
  }
};

#endif
