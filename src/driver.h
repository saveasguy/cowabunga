#ifndef KALEIDOC_SRC_DRIVER_H_
#define KALEIDOC_SRC_DRIVER_H_

#include <istream>
#include <memory>
#include <string>

namespace kaleidoc {

enum TokenId {
  kEofId = 0,
  kDefId = 1,
  kExternId = 2,
  kIdentifierId = 3,
  kNumberId = 4,
  kAssignmentId = 5,
  kPlusId = 6,
  kMinusId = 7
};

enum TokenPriority { kUnmatched = 0, kNormal = 1, kHigh = 2 };

template<class T> class IClonable {
 public:
  virtual std::unique_ptr<T> Clone() const = 0;
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

  virtual std::unique_ptr<Token> GetToken(const std::string &word) const = 0;

  virtual void Flush() = 0;

  virtual ~LexemeAnalyzer() = default;
};

class Lexer : public IClonable<Lexer> {
 public:
  virtual Lexer &AddAnalyzer(std::unique_ptr<LexemeAnalyzer> analyzer) = 0;

  virtual std::unique_ptr<Token> NextToken(std::istream &input) = 0;

  virtual ~Lexer() = default;
};

class Driver {};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_DRIVER_H_