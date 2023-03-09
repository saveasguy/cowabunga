#include "common/IPrintable.h"

namespace cb {

namespace common {

std::ostream &operator<<(std::ostream &Out, const IPrintable &RHS) {
  RHS.print(Out);
  return Out;
}

} // namespace common

} // namespace cb