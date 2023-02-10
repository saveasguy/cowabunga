#ifndef KALEIDOC_SRC_LEXER_H_
#define KALEIDOC_SRC_LEXER_H_

#include <exception>
#include <istream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "driver.h"
#include "token.h"

namespace kaleidoc {

class UnknownLexemeException : public std::exception {
 public:
  UnknownLexemeException(std::string msg);

  const char *what() const noexcept override;

  ~UnknownLexemeException() override = default;

 private:
  std::string msg_;
};

class FullTextLexer : public Lexer {
 public:
  FullTextLexer() = default;

  FullTextLexer(const FullTextLexer &rhs);

  FullTextLexer(FullTextLexer &&rhs) noexcept = default;

  FullTextLexer &operator=(const FullTextLexer &rhs);

  FullTextLexer &operator=(FullTextLexer &&rhs) noexcept = default;

  Lexer &AddTokenizer(std::unique_ptr<Tokenizer> tokenizer) override;

  std::vector<Token> ProduceTokens(std::istream &input) const override;

  std::unique_ptr<Lexer> Clone() const override;

  ~FullTextLexer() override = default;

 private:
  Token FindBestToken(std::string_view word) const;

  std::vector<std::unique_ptr<Tokenizer>> tokenizers_;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_LEXER_H_
