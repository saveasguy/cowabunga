#include "cowabunga/CBC/ASTBuilder.h"
#include "cowabunga/CBC/ASTNodes.h"
#include "cowabunga/Lexer/Token.h"

#include <cassert>
#include <memory>
#include <vector>

using namespace cb;

void ASTBuilder::createVariable(const Token &Tok) {
  CreatedExpressions.push_back(std::make_unique<VariableASTNode>(Tok));
}

void ASTBuilder::createIntegralNumber(const Token &Tok) {
  CreatedExpressions.push_back((std::make_unique<IntegralNumberASTNode>(Tok)));
}

void ASTBuilder::createCompoundExpression(std::string ExpressionSeparator) {
  assert(!CreatedExpressions.empty() && "There should be at least one Node");
  std::vector<std::unique_ptr<IASTNode>> ASTNodes;
  ASTNodes.reserve(CreatedExpressions.size());
  auto It = CreatedExpressions.rbegin();
  auto ItEnd = CreatedExpressions.rend();
  for (; It != ItEnd; ++It) {
    ASTNodes.push_back(std::move(*It));
  }
  CreatedExpressions.clear();
  CreatedExpressions.push_back(std::make_unique<CompoundExpressionASTNode>(
      ExpressionSeparator, std::move(ASTNodes)));
}

void ASTBuilder::createAssignmentExpression(std::string Assignment) {
  assert(CreatedExpressions.size() >= 2 &&
         "There should be at leas 2 Nodes for Assignment");
  auto LHS = std::move(CreatedExpressions.back());
  CreatedExpressions.pop_back();
  auto RHS = std::move(CreatedExpressions.back());
  CreatedExpressions.pop_back();
  CreatedExpressions.push_back(std::make_unique<AssignmentExpressionASTNode>(
      Assignment, std::move(LHS), std::move(RHS)));
}

void ASTBuilder::createParameterList() { CreatedParameterLists.push({}); }

void ASTBuilder::createParameter() {
  assert(!CreatedParameterLists.empty() &&
         "There should be at least 1 Parameter List");
  assert(!CreatedExpressions.empty() && "There should be at least 1 Node");
  CreatedParameterLists.top().push_back(std::move(CreatedExpressions.back()));
  CreatedExpressions.pop_back();
}

void ASTBuilder::createFunctionCall(const Token &Tok) {
  assert(!CreatedParameterLists.empty() &&
         "There should be at least 1 Parameter List");
  auto It = CreatedParameterLists.top().rbegin();
  auto ItEnd = CreatedParameterLists.top().rend();
  std::vector<std::unique_ptr<IASTNode>> Params;
  Params.reserve(CreatedParameterLists.top().size());
  for (; It != ItEnd; ++It) {
    Params.push_back(std::move(*It));
  }
  CreatedParameterLists.pop();
  CreatedExpressions.push_back(
      std::make_unique<CallExpressionASTNode>(Tok, std::move(Params)));
}

std::unique_ptr<IASTNode> ASTBuilder::release() {
  assert(CreatedExpressions.size() == 1 &&
         "There should be one Top Level ASTNode");
  auto TopLevelNode = std::move(CreatedExpressions.back());
  CreatedExpressions.pop_back();
  return TopLevelNode;
}
