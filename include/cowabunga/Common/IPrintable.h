#ifndef COWABUNGA_COMMON_IPRINTABLE_H
#define COWABUNGA_COMMON_IPRINTABLE_H

#include <ostream>

namespace cb {

class IPrintable {
public:
  virtual void print(std::ostream &Out) const = 0;

  virtual ~IPrintable();
};

std::ostream &operator<<(std::ostream &Out, const IPrintable &RHS);

} // namespace cb

#endif // COWABUNGA_COMMON_IPRINTABLE_H
