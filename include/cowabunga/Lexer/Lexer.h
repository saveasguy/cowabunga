#ifndef COWABUNGA_LEXER_LEXER_H
#define COWABUNGA_LEXER_LEXER_H

#include "cowabunga/Lexer/Token.h"
#include "cowabunga/Lexer/TokenizerProxy.h"

#include <memory>
#include <optional>
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

  std::optional<std::vector<Token>> produceTokens(std::istream &Input);

private:
  std::pair<Token, std::string> findBestToken(std::string_view Word) const;

  std::vector<std::unique_ptr<ITokenizerProxy>> Tokenizers;
};

} // namespace cb

#endif // COWABUNGA_LEXER_LEXER_H
