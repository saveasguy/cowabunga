#ifndef KALEIDOC_SRC_TOKEN_H_
#define KALEIDOC_SRC_TOKEN_H_

#include <memory>
#include <string>

#include "driver.h"

namespace kaleidoc {

class EofToken : public Token {
 public:
  TokenId GetTokenId() const noexcept override;

  std::string GetValue() const noexcept override;

  std::unique_ptr<Token> Clone() const override;
};

class IdentifierToken : public Token {
 public:
  IdentifierToken(const std::string &name);

  TokenId GetTokenId() const noexcept override;

  std::string GetValue() const noexcept override;

  std::unique_ptr<Token> Clone() const override;

 private:
  std::string name_;
};

class KeywordToken : public Token {
 public:
  KeywordToken(TokenId token_id, const std::string &name);

  TokenId GetTokenId() const noexcept override;

  std::string GetValue() const noexcept override;

  std::unique_ptr<Token> Clone() const override;

 private:
  TokenId token_id_;
  std::string name_;
};

class IntegralNumberToken : public Token {
 public:
  IntegralNumberToken(const std::string &value);

  TokenId GetTokenId() const noexcept override;

  std::string GetValue() const noexcept override;

  std::unique_ptr<Token> Clone() const override;

 private:
  std::string value_;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_TOKEN_H_
