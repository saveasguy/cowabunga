#ifndef CBC_ASTPASSES_H
#define CBC_ASTPASSES_H

#include "cbc/ASTNodes.h"
#include <ostream>

namespace cb {

class IASTPass {
public:
  virtual void accept(VariableASTNode &Node) = 0;

  virtual void accept(IntegralNumberASTNode &Node) = 0;

  virtual void accept(BinaryExpressionASTNode &Node) = 0;

  virtual void accept(ParenthesizedExpressionASTNode &Node) = 0;

  virtual void accept(CompoundExpressionASTNode &Node) = 0;

  virtual ~IASTPass() = default;
};

class ASTPrinter : public IASTPass {
public:
  ASTPrinter(std::ostream &OStream);

  void accept(VariableASTNode &Node) override;

  void accept(IntegralNumberASTNode &Node) override;

  void accept(BinaryExpressionASTNode &Node) override;

  void accept(ParenthesizedExpressionASTNode &Node) override;

  void accept(CompoundExpressionASTNode &Node) override;

private:
  void printTreeBranch() const;

  std::ostream &Out;
  size_t Depth = 0;
};

} // namespace cb

#endif
