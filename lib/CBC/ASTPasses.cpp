#include "cowabunga/CBC/ASTPasses.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Support/Host.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

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
  for (auto &Expression : Node.Expressions) {
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

ASTCodeGen::ASTCodeGen()
    : Context(), MainModule("Cowabunga", Context), Builder(Context) {
  MainModule.setTargetTriple(llvm::sys::getDefaultTargetTriple());
  Intrinsics["add"] = &ASTCodeGen::codeGenAdditionIntrinsic;
  Intrinsics["sub"] = &ASTCodeGen::codeGenSubstractionIntrinsic;
  llvm::FunctionType *FT =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), false);
  F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main",
                             &MainModule);
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(Context, "entry", F);
  Builder.SetInsertPoint(BB);
}

void ASTCodeGen::accept(VariableASTNode &Node) {
  if (NamedValues.count(Node.Name)) {
    CodeGeneratedValues.push_back(NamedValues[Node.Name]);
    LastUsedLValue = &NamedValues[Node.Name];
    return;
  }
  llvm::Value *Zero = llvm::ConstantInt::get(Context, llvm::APInt(64, "0", 10));
  NamedValues[Node.Name] = Zero;
  LastUsedLValue = &NamedValues[Node.Name];
  CodeGeneratedValues.push_back(Zero);
}

void ASTCodeGen::accept(IntegralNumberASTNode &Node) {
  CodeGeneratedValues.push_back(
      llvm::ConstantInt::get(Context, llvm::APInt(64, Node.Value, 10)));
}

void ASTCodeGen::accept(AssignmentExpressionASTNode &Node) {
  Node.LHS->acceptASTPass(*this);
  auto **RefToLValue = LastUsedLValue;
  CodeGeneratedValues.pop_back();
  Node.RHS->acceptASTPass(*this);
  *RefToLValue = CodeGeneratedValues.back();
}

void ASTCodeGen::accept(CompoundExpressionASTNode &Node) {
  for (auto &Expression : Node.Expressions) {
    CodeGeneratedValues.clear();
    Expression->acceptASTPass(*this);
  }
}

void ASTCodeGen::accept(CallExpressionASTNode &Node) {
  if (Intrinsics.count(Node.FuncName) == 1) {
    auto OldCodeGeneratedValues = CodeGeneratedValues;
    CodeGeneratedValues.clear();
    for (auto &Param : Node.Parameters) {
      Param->acceptASTPass(*this);
    }
    auto IntrinsicHandler = Intrinsics[Node.FuncName];
    auto *ReturnValue = (this->*IntrinsicHandler)(CodeGeneratedValues);
    CodeGeneratedValues = OldCodeGeneratedValues;
    CodeGeneratedValues.push_back(ReturnValue);
  } else {
    std::cerr << "Unsupported intrinsic";
  }
}

llvm::Value *
ASTCodeGen::codeGenAdditionIntrinsic(std::vector<llvm::Value *> Params) {
  auto *LHS = Params.front();
  Params.erase((Params.begin()));
  auto ItRHS = Params.begin();
  for (; ItRHS != Params.end(); ++ItRHS) {
    LHS = Builder.CreateAdd(LHS, *ItRHS);
  }
  return LHS;
}

llvm::Value *
ASTCodeGen::codeGenSubstractionIntrinsic(std::vector<llvm::Value *> Params) {
  auto *LHS = Params.front();
  Params.erase((Params.begin()));
  auto ItRHS = Params.begin();
  for (; ItRHS != Params.end(); ++ItRHS) {
    LHS = Builder.CreateSub(LHS, *ItRHS);
  }
  return LHS;
}

int ASTCodeGen::compile(std::string IRFileName, std::string ExecutableName) {
  assert(CodeGeneratedValues.size() == 1 &&
         "There should be some generated value");
  auto *DestTy = llvm::IntegerType::getInt32Ty(Context);
  auto *RetVal = llvm::ConstantInt::get(DestTy, 0);
  auto *Result = CodeGeneratedValues.back();
  CodeGeneratedValues.clear();
  auto *CharPtrTy =
      llvm::PointerType::get(llvm::IntegerType::get(Context, 8), 0);
  auto *FprintfTy =
      llvm::FunctionType::get(llvm::IntegerType::get(Context, 32), true);
  llvm::Function *FPrintf = llvm::Function::Create(
      FprintfTy, llvm::GlobalValue::ExternalLinkage, "printf", MainModule);
  FPrintf->setCallingConv(llvm::CallingConv::C);
  auto *FormatString = Builder.CreateGlobalStringPtr("%lld\n");
  Builder.CreateCall(FPrintf, {FormatString, Result});
  Builder.CreateRet(RetVal);
  assert(!llvm::verifyFunction(*F, &llvm::outs()) &&
         "Couldn't verify LLVM IR function");
  std::error_code ErrorCode;
  llvm::raw_fd_stream IRFile(IRFileName, ErrorCode);
  MainModule.print(IRFile, nullptr);
  std::string ClangDriverCommand =
      "clang " + IRFileName + " -o " + ExecutableName;
  return std::system(ClangDriverCommand.c_str());
}
