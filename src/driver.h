#ifndef KALEIDOC_SRC_DRIVER_H_
#define KALEIDOC_SRC_DRIVER_H_

#include <istream>
#include <string>

namespace kaleidoc {

enum TokenId {
  kEofId = 0,
  kDefId = 1,
  kExternId = 2,
  kIdentifierId = 3,
  kNumberId = 4,
  kBinaryOperationId = 5
};

enum TokenPriority { kUnmatched = 0, kNormal = 1, kKeyword = 2 };

template<class T> class IClonable {
 public:
  virtual T *Clone() const = 0;
};

class Token : public IClonable<Token> {
 public:
  virtual TokenId GetToken() const noexcept = 0;

  virtual std::string GetValue() const noexcept = 0;

  virtual ~Token() = default;

  int a;
};

class LexemeAnalyzer : public IClonable<LexemeAnalyzer> {
 public:
  TokenPriority CheckWholeWord(const std::string &word);

  virtual TokenPriority CheckNextChar(char c) = 0;

  virtual Token *GetToken(const std::string &word) const = 0;

  virtual void Flush() = 0;

  virtual ~LexemeAnalyzer() = default;
};

class Lexer : public IClonable<Lexer> {
 public:
  virtual Lexer *AddAnalyzer(const LexemeAnalyzer *ananlyer) = 0;

  virtual Token *NextToken(std::istream &input) = 0;

  virtual ~Lexer() = default;
};

class Driver {};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_DRIVER_H_