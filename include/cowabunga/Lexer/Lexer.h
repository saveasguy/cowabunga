#ifndef COWABUNGA_LEXER_LEXER_H
#define COWABUNGA_LEXER_LEXER_H

#include "cowabunga/Lexer/Token.h"
#include "cowabunga/Lexer/TokenizerProxy.h"

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

  template <class Tokenizer> Lexer &addTokenizer(Tokenizer TokenizerImpl) {
    Tokenizers.push_back(
        std::make_unique<TokenizerProxy<Tokenizer>>(std::move(TokenizerImpl)));
    return *this;
  }

  std::vector<Token> tokenize(std::istream &Input,
                              const std::string &FileName = "");

private:
  std::pair<std::vector<Token>, bool> tokenizeLine(const std::string &Line);

  static size_t skipWhitespace(const std::string &Line, size_t Pos);

  std::pair<Token, size_t> findBestToken(std::string_view Word) const;

  bool errorOnUnrecognizedToken(const std::pair<Token, size_t> &Result,
                                const std::string &Line,
                                size_t WordLength) const;

  std::vector<std::unique_ptr<ITokenizerProxy>> Tokenizers;
  std::shared_ptr<const std::string> File;
  size_t LineNumber;
  size_t ColumnNumber;
};

} // namespace cb

#endif // COWABUNGA_LEXER_LEXER_H
