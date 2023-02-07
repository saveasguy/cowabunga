#ifndef KALEIDOC_SRC_LEXEME_ANALYZER_H_
#define KALEIDOC_SRC_LEXEME_ANALYZER_H_

#include <memory>
#include <string>

#include "driver.h"

namespace kaleidoc {

class IdentifierAnalyzer : public LexemeAnalyzer {
 public:
  TokenPriorityId CheckNextChar(char c) override;

  std::unique_ptr<Token> GetTokenId(const std::string &word) const override;

  void Flush() override;

  std::unique_ptr<LexemeAnalyzer> Clone() const override;

 private:
  bool flushed_ = true;
};

class KeywordAnalyzer : public LexemeAnalyzer {
 public:
  KeywordAnalyzer(TokenId token_id, const std::string &keyword);

  TokenPriorityId CheckNextChar(char c) override;

  std::unique_ptr<Token> GetTokenId(const std::string &word) const override;

  void Flush() override;

  std::unique_ptr<LexemeAnalyzer> Clone() const override;

 private:
  TokenId token_id_;
  std::string keyword_;
  std::string current_input_;
};

class IntegralNumberAnalyzer : public LexemeAnalyzer {
 public:
  TokenPriorityId CheckNextChar(char c) override;

  std::unique_ptr<Token> GetTokenId(const std::string &word) const override;

  void Flush() override;

  std::unique_ptr<LexemeAnalyzer> Clone() const override;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_LEXEME_ANALYZER_H_
