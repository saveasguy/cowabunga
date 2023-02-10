#ifndef KALEIDOC_SRC_DRIVER_H_
#define KALEIDOC_SRC_DRIVER_H_

#include <istream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace kaleidoc {

enum TokenId {
  kEof,
  kIdentifier,
  kIntegralNumber,
  kExpressionSeparator,
  kArgumentSeparator,
  kAssignment,
  kPlus,
  kMinus,
  kDefinition,
  kExternalDeclaration,
  kBodyBegin,
  kBodyEnd
};

enum TokenPriority { kUnmatched = 0, kNormal = 1, kHigh = 2 };

enum MetadataType {};  // Not implemented yet

enum OperatorId {
  kAssignmentOp = TokenId::kAssignment,
  kPlusOp = TokenId::kPlus,
  kMinusOp = TokenId::kMinus
};

class Token;

template<class T> class IClonable {
 public:
  virtual std::unique_ptr<T> Clone() const = 0;
};

class Tokenizer : public IClonable<Tokenizer> {
 public:
  virtual Token Tokenize(std::string_view word) const = 0;

  virtual ~Tokenizer() = default;
};

class Lexer : public IClonable<Lexer> {
 public:
  virtual Lexer &AddTokenizer(std::unique_ptr<Tokenizer> analyzer) = 0;

  virtual std::vector<Token> ProduceTokens(std::istream &input) const = 0;

  virtual ~Lexer() = default;
};

class AstNode : public IClonable<AstNode> {
 public:
  virtual ~AstNode() = default;
};

class Parser : public IClonable<Parser> {
 public:
  virtual std::unique_ptr<AstNode> Parse(std::vector<Token>::const_iterator begin,
                                 std::vector<Token>::const_iterator end) const = 0;

  virtual ~Parser() = default;
};

class Driver {};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_DRIVER_H_