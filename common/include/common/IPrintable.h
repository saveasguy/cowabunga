#ifndef COMMON_IPRINTABLE_H
#define COMMON_IPRINTABLE_H

#include <ostream>

namespace cb {

namespace common {

class IPrintable {
public:
  virtual void print(std::ostream &Out) const = 0;
};

std::ostream &operator<<(std::ostream &Out, const IPrintable &RHS);

} // namespace common

} // namespace cb

#endif
