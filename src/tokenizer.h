#ifndef KALEIDOC_SRC_TOKENIZER_H_
#define KALEIDOC_SRC_TOKENIZER_H_

#include <memory>
#include <string>
#include <string_view>

#include "driver.h"

namespace kaleidoc {

class EofTokenizer : public Tokenizer {
 public:
  Token Tokenize(std::string_view word) const override;

  std::unique_ptr<Tokenizer> Clone() const override;
};

class IdentifierTokenizer : public Tokenizer {
 public:
  Token Tokenize(std::string_view word) const override;

  std::unique_ptr<Tokenizer> Clone() const override;
};

class KeywordTokenizer : public Tokenizer {
 public:
  KeywordTokenizer(TokenId token_id, std::string keyword);

  Token Tokenize(std::string_view word) const override;

  std::unique_ptr<Tokenizer> Clone() const override;

 private:
  TokenId token_id_;
  std::string stringified_keyword_;
};

class IntegralNumberTokenizer : public Tokenizer {
 public:
  Token Tokenize(std::string_view word) const override;

  std::unique_ptr<Tokenizer> Clone() const override;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_LEXEME_ANALYZER_H_
