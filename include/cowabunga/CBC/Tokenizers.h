#ifndef COWABUNGA_CBC_TOKENIZERS_H
#define COWABUNGA_CBC_TOKENIZERS_H

#include "cowabunga/Lexer/ITokenizer.h"
#include "cowabunga/Lexer/Token.h"

#include <string_view>
#include <unordered_map>

namespace cb {

enum TokenID {
  TID_ExpressionSeparator,
  TID_ArgumentSeparator,
  TID_Assignment,
  TID_LambdaBegin,
  TID_LambdaEnd,
  TID_OpenParantheses,
  TID_CloseParantheses,
  TID_Identifier,
  TID_IntegralNumber
};

class IdentifierTokenizer : public ITokenizer {
public:
  std::pair<std::optional<Token>, size_t> tokenize(std::string_view Word) override;

  int getTokenID() const override;

  std::string getLexemeString() const override;
};

class IntegralNumberTokenizer : public ITokenizer {
public:
  std::pair<std::optional<Token>, size_t> tokenize(std::string_view Word) override;

  int getTokenID() const override;

  std::string getLexemeString() const override;
};

class KeywordTokenizer : public ITokenizer {
public:
  KeywordTokenizer(TokenID TokId, std::string Keyword);

  std::pair<std::optional<Token>, size_t> tokenize(std::string_view Word) override;

  int getTokenID() const override;

  std::string getLexemeString() const override;

private:
  TokenID ID;
  std::string KeywordString;
};

} // namespace cb

#endif // COWABUNGA_CBC_TOKENIZERS_H
