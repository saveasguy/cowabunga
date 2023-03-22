#ifndef COWABUNGA_CBC_TOKENIZERS_H
#define COWABUNGA_CBC_TOKENIZERS_H

#include "cowabunga/CBC/Definitions.h"
#include "cowabunga/Lexer/ITokenizer.h"
#include "cowabunga/Lexer/Token.h"

#include <string_view>

namespace cb {

class IdentifierTokenizer : public ITokenizer {
public:
  std::pair<Token, size_t> tokenize(std::string_view Word) override;
};

class IntegralNumberTokenizer : public ITokenizer {
public:
  std::pair<Token, size_t> tokenize(std::string_view Word) override;
};

class KeywordTokenizer : public ITokenizer {
public:
  KeywordTokenizer(TokenID TokId, std::string Keyword);

  std::pair<Token, size_t> tokenize(std::string_view Word) override;

private:
  TokenID ID;
  std::string KeywordString;
};

} // namespace cb

#endif // COWABUNGA_CBC_TOKENIZERS_H
