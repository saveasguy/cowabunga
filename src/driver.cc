#include "driver.h"

namespace kaleidoc {

void AstNode::Print(std::ostream &out) const { out << "AST Node"; }

std::ostream &operator<<(std::ostream &out, const AstNode &node) {
  node.Print(out);
  return out;
}

}  // namespace kaleidoc
