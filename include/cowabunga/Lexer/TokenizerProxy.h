#ifndef COWABUNGA_LEXER_TOKENIZERPROXY_H
#define COWABUNGA_LEXER_TOKENIZERPROXY_H

#include "cowabunga/Common/IClonableMixin.h"
#include "cowabunga/Lexer/ITokenizer.h"
#include "cowabunga/Lexer/Token.h"

#include <memory>
#include <string_view>
#include <type_traits>

namespace cb {

class ITokenizerProxy : public ITokenizer {
public:
  virtual std::unique_ptr<ITokenizerProxy> clone() const = 0;

  ~ITokenizerProxy() override;
};

template <class Tokenizer,
          class = std::enable_if<std::is_base_of_v<ITokenizer, Tokenizer>>>
class TokenizerProxy final
    : public IClonableMixin<ITokenizerProxy, TokenizerProxy<Tokenizer>> {
public:
  TokenizerProxy(Tokenizer Impl) : TokenizerImpl(Impl) {}

  std::pair<Token, size_t> tokenize(std::string_view Word) override {
    return TokenizerImpl.tokenize(Word);
  }

private:
  Tokenizer TokenizerImpl;
};

} // namespace cb

#endif // COWABUNGA_LEXER_TOKENIZERPROXY_H
