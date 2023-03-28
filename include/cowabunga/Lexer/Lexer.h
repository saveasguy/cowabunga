#ifndef COWABUNGA_LEXER_LEXER_H
#define COWABUNGA_LEXER_LEXER_H

#include "cowabunga/Lexer/Token.h"
#include "cowabunga/Lexer/TokenizerProxy.h"

#include <unordered_map>
#include <vector>

namespace cb {

using TokenIterator = std::vector<Token>::const_iterator;

class Lexer final {
public:
  Lexer() = default;

  Lexer(const Lexer &RHS);

  Lexer(Lexer &&RHS) noexcept = default;

  Lexer &operator=(const Lexer &RHS);

  Lexer &operator=(Lexer &&RHS) noexcept = default;

  template <class TTokenizer> Lexer &addTokenizer(TTokenizer &&TokenizerImpl) {
    LexemeStringMapping[TokenizerImpl.getTokenID()] = TokenizerImpl.getLexemeString();
    Tokenizers.push_back(
        std::make_unique<TokenizerProxy<TTokenizer>>(std::forward<TTokenizer>(TokenizerImpl)));
    return *this;
  }

  std::vector<Token> tokenize(std::istream &Input,
                              const std::string &FileName = "");

  std::string getTokenLexeme(int ID) const;

private:
  std::unordered_map<int, std::string> LexemeStringMapping;
  std::vector<std::unique_ptr<ITokenizerProxy>> Tokenizers;
};

} // namespace cb

#endif // COWABUNGA_LEXER_LEXER_H
