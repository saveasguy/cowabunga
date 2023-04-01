#ifndef COWABUNGA_CBC_ASTBUILDER_H
#define COWABUNGA_CBC_ASTBUILDER_H

#include "cowabunga/CBC/ASTNodes.h"
#include "cowabunga/Lexer/Token.h"

#include <memory>
#include <stack>
#include <vector>

namespace cb {

class ASTBuilder final {
public:
  void createVariable(const Token &Tok);

  void createIntegralNumber(const Token &Tok);

  void createCompoundExpression(std::string ExpressionSeparator);

  void createAssignmentExpression(std::string Assignment);

  void createParameterList();

  void createParameter();

  void createFunctionCall(const Token &Tok);

  std::unique_ptr<IASTNode> release();
private:
  std::stack<std::vector<std::unique_ptr<IASTNode>>> CreatedParameterLists;
  std::vector<std::unique_ptr<IASTNode>> CreatedExpressions;
};

} // namespace cb

#endif // COWABUNGA_CBC_ASTBUILDER_H
