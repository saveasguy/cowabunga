#include "cowabunga/Common/IPrintable.h"

using namespace cb;

IPrintable::~IPrintable() {}

std::ostream &cb::operator<<(std::ostream &Out, const IPrintable &RHS) {
  RHS.print(Out);
  return Out;
}
