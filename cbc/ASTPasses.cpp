#include "cbc/ASTPasses.h"
#include "common/Metadata.h"

namespace cb {

ASTPrinter::ASTPrinter(std::ostream &OStream) : Out(OStream) {}

void ASTPrinter::accept(VariableASTNode &Node) { Out << Node; }

void ASTPrinter::accept(IntegralNumberASTNode &Node) { Out << Node; }

void ASTPrinter::accept(BinaryExpressionASTNode &Node) {
  Out << Node << "\n";
  ++Depth;
  printTreeBranch();
  Node.LHS->acceptASTPass(*this);
  Out << "\n";
  printTreeBranch();
  Node.RHS->acceptASTPass(*this);
  --Depth;
}

void ASTPrinter::accept(ParenthesizedExpressionASTNode &Node) {
  Out << Node << "\n";
  ++Depth;
  printTreeBranch();
  Node.Expression->acceptASTPass(*this);
  --Depth;
}

void ASTPrinter::accept(CompoundExpressionASTNode &Node) {
  printTreeBranch();
  Node.First->acceptASTPass(*this);
  printTreeBranch();
  Out << "\n" << Node << "\n";
  printTreeBranch();
  Node.Second->acceptASTPass(*this);
}

void ASTPrinter::printTreeBranch() const {
  for (size_t I = 0; I < Depth; ++I) {
    Out << "|--";
  }
}

} // namespace cb