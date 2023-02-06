#ifndef KALEIDOC_SRC_LEXER_H_
#define KALEIDOC_SRC_LEXER_H_

#include <exception>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "driver.h"

namespace kaleidoc {

class UnknownLexemeException : public std::exception {
 public:
  UnknownLexemeException(const std::string &msg);

  const char *what() const noexcept override;

  ~UnknownLexemeException() override = default;

 private:
  std::string msg_;
};

class SymbolicLexer : public Lexer {
 public:
  SymbolicLexer() = default;

  SymbolicLexer(const SymbolicLexer &rhs);

  SymbolicLexer(SymbolicLexer &&rhs) noexcept = default;

  SymbolicLexer &operator=(const SymbolicLexer &rhs);

  SymbolicLexer &operator=(SymbolicLexer &&rhs) noexcept = default;

  Lexer &AddAnalyzer(std::unique_ptr<LexemeAnalyzer> analyzer) override;

  std::unique_ptr<Token> NextToken(std::istream &input) override;

  std::unique_ptr<Lexer> Clone() const override;

  ~SymbolicLexer() override = default;

 private:
  std::unique_ptr<Token> GetBestMatchedToken(std::istream &input);

  static std::string TryGetWordTail(std::istream &input, LexemeAnalyzer *analyzer);

  static void SkipBlankSpace(std::istream &input);

  std::vector<std::unique_ptr<LexemeAnalyzer>> analyzers_;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_LEXER_H_
