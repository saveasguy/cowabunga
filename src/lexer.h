#ifndef KALEIDOC_SRC_LEXER_H_
#define KALEIDOC_SRC_LEXER_H_

#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "driver.h"

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

  ~FullTextLexer() override = default;

 private:
  Token FindBestToken(std::string_view word) const;

  std::vector<std::unique_ptr<Tokenizer>> tokenizers_;
};

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

class Token final {
 public:
  Token();

  Token(TokenId token_id, std::string matched_string,
        TokenPriority token_priority = TokenPriority::kNormal);

  void AddMetadata(MetadataType type, std::string value);

  bool Equals(const Token &rhs) const;

  bool Less(const Token &rhs) const;

  operator bool() const;

  TokenId id() const;

  TokenPriority priority() const;

  std::string stringified() const;

  const std::map<MetadataType, std::string> &metadata() const;

 private:
  std::map<MetadataType, std::string> metadata_;
  std::string stringified_;
  TokenId id_;
  TokenPriority priority_;
};

bool operator==(const Token &lhs, const Token &rhs);

bool operator!=(const Token &lhs, const Token &rhs);

bool operator>(const Token &lhs, const Token &rhs);

bool operator>=(const Token &lhs, const Token &rhs);

bool operator<(const Token &lhs, const Token &rhs);

bool operator<=(const Token &lhs, const Token &rhs);

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_LEXER_H_
