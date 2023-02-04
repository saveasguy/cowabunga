#ifndef KALEIDOC_SRC_LEXEME_ANALYZER_H_
#define KALEIDOC_SRC_LEXEME_ANALYZER_H_

#include "driver.h"

namespace kaleidoc {

class IdentifierAnalyzer : public LexemeAnalyzer {
 public:
  TokenPriority CheckNextChar(char c) override;

  Token *GetToken(const std::string &word) const override;

  void Flush() override;

  LexemeAnalyzer *Clone() const override;

 private:
  bool flushed_ = true;
};

class KeywordAnalyzer : public LexemeAnalyzer {
 public:
  KeywordAnalyzer(TokenId token_id, const std::string &keyword);

  TokenPriority CheckNextChar(char c) override;

  Token *GetToken(const std::string &word) const override;

  void Flush() override;

  LexemeAnalyzer *Clone() const override;

 private:
  TokenId token_id_;
  std::string keyword_;
  std::string current_input_;
};

class NumberAnalyzer : public LexemeAnalyzer {
 public:
  TokenPriority CheckNextChar(char c) override;

  Token *GetToken(const std::string &word) const override;

  void Flush() override;

  LexemeAnalyzer *Clone() const override;

 private:
  bool point_used_ = false;
};

class BinaryOperatorAnalyzer : public LexemeAnalyzer {
 public:
  BinaryOperatorAnalyzer(const std::string &operation);

  TokenPriority CheckNextChar(char c) override;

  Token *GetToken(const std::string &word) const override;

  void Flush() override;

  LexemeAnalyzer *Clone() const override;

 private:
  std::string operator_;
  std::string current_input_;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_LEXEME_ANALYZER_H_
