#ifndef COWABUNGA_CBC_ASTPASSES_H
#define COWABUNGA_CBC_ASTPASSES_H

#include "cowabunga/CBC/ASTNodes.h"

#include <ostream>

namespace cb {

class IASTPass {
public:
  virtual void accept(VariableASTNode &Node) = 0;

  virtual void accept(IntegralNumberASTNode &Node) = 0;

  virtual void accept(AssignmentExpressionASTNode &Node) = 0;
  
  virtual void accept(CompoundExpressionASTNode &Node) = 0;

  virtual void accept(CallExpressionASTNode &Node) = 0;

  virtual ~IASTPass();
};

class ASTPrinter : public IASTPass {
public:
  ASTPrinter(std::ostream &OStream);

  void accept(VariableASTNode &Node) override;

  void accept(IntegralNumberASTNode &Node) override;
  
  void accept(AssignmentExpressionASTNode &Node) override;
  
  void accept(CompoundExpressionASTNode &Node) override;

  void accept(CallExpressionASTNode &Node) override;

private:
  void printTreeBranch() const;

  std::ostream &Out;
  size_t Depth = 0;
};

} // namespace cb

#endif // COWABUNGA_CBC_ASTPASSES_H
