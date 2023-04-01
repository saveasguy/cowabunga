#include "cowabunga/CBC/ASTPasses.h"

using namespace cb;

IASTPass::~IASTPass() {}

ASTPrinter::ASTPrinter(std::ostream &OStream) : Out(OStream) {}

void ASTPrinter::accept(VariableASTNode &Node) { Out << Node; }

void ASTPrinter::accept(IntegralNumberASTNode &Node) { Out << Node; }

void ASTPrinter::accept(AssignmentExpressionASTNode &Node) {
  Out << Node << "\n";
  ++Depth;
  printTreeBranch();
  Node.LHS->acceptASTPass(*this);
  Out << "\n";
  printTreeBranch();
  Node.RHS->acceptASTPass(*this);
  --Depth;
}

void ASTPrinter::accept(CompoundExpressionASTNode &Node) {
  Out << Node;
  ++Depth;
  for (auto &Expression: Node.Expressions) {
    Out << "\n";
    printTreeBranch();
    Expression->acceptASTPass(*this);
  }
  --Depth;
}

void ASTPrinter::accept(CallExpressionASTNode &Node) {
  Out << Node;
  ++Depth;
  for (auto &Param : Node.Parameters) {
    Out << "\n";
    printTreeBranch();
    Param->acceptASTPass(*this);
  }
  --Depth;
}

void ASTPrinter::printTreeBranch() const {
  for (size_t I = 0; I < Depth; ++I) {
    Out << "|--";
  }
}
