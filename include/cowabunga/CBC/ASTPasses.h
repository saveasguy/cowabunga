#ifndef COWABUNGA_CBC_ASTPASSES_H
#define COWABUNGA_CBC_ASTPASSES_H

#include "cowabunga/CBC/ASTNodes.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <ostream>
#include <unordered_map>

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

class ASTPrinter final : public IASTPass {
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

class ASTCodeGen final : public IASTPass {
public:
  ASTCodeGen();

  void accept(VariableASTNode &Node) override;

  void accept(IntegralNumberASTNode &Node) override;

  void accept(AssignmentExpressionASTNode &Node) override;

  void accept(CompoundExpressionASTNode &Node) override;

  void accept(CallExpressionASTNode &Node) override;

  int compile(std::string IRFileName = "main.ll",
               std::string ExecutableFileName = "main");

private:
  llvm::Value *codeGenAdditionIntrinsic(std::vector<llvm::Value *> Params);

  llvm::Value *codeGenSubstractionIntrinsic(std::vector<llvm::Value *> Params);

  llvm::LLVMContext Context;
  llvm::Module MainModule;
  llvm::IRBuilder<> Builder;
  std::unordered_map<std::string,
                     llvm::Value *(ASTCodeGen::*)(std::vector<llvm::Value *>)>
      Intrinsics;
  std::vector<llvm::Value *> CodeGeneratedValues;
  std::unordered_map<std::string, llvm::Value *> NamedValues;
  llvm::Value **LastUsedLValue;
  llvm::Function *F;
};

} // namespace cb

#endif // COWABUNGA_CBC_ASTPASSES_H
