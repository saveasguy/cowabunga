#ifndef KALEIDOC_SRC_DRIVER_H_
#define KALEIDOC_SRC_DRIVER_H_

#include <memory>
#include <ostream>
#include <vector>

namespace kaleidoc {

enum TokenId {
  kEof,
  kIdentifier,
  kIntegralNumber,
  kExpressionSeparator,
  kArgumentSeparator,
  kBinaryOperatorsRangeBegin,
  kAssignment,
  kPlus,
  kMinus,
  kShiftLeft,
  kShiftRight,
  kBinaryOperatorsRangeEnd,
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
  kMinusOp = TokenId::kMinus,
  kShiftLeftOp = TokenId::kShiftLeft,
  kShiftRightOp = TokenId::kShiftRight
};

class Token;

template<class T> class IClonable {
 public:
  virtual std::unique_ptr<T> Clone() const = 0;
};

class IPrintable {
 public:
  virtual void Print(std::ostream &out) const = 0;
};

class Tokenizer : public IClonable<Tokenizer> {
 public:
  virtual Token Tokenize(std::string_view word) const = 0;

  virtual ~Tokenizer() = default;
};

class Lexer {
 public:
  virtual Lexer &AddTokenizer(std::unique_ptr<Tokenizer> analyzer) = 0;

  virtual std::vector<Token> ProduceTokens(std::istream &input) const = 0;

  virtual ~Lexer() = default;
};

class AstNode : public IClonable<AstNode>, public IPrintable {
 public:
  void Print(std::ostream &out) const override;

  virtual ~AstNode() = default;
};

std::ostream &operator<<(std::ostream &out, const AstNode &node);

class AstBuilder : public IClonable<AstBuilder> {
 public:
  virtual std::pair<std::unique_ptr<AstNode>,
                    std::vector<Token>::const_iterator>
  Build(std::vector<Token>::const_iterator begin,
        std::vector<Token>::const_iterator end) const = 0;

  virtual ~AstBuilder() = default;
};

class Parser {
 public:
  virtual AstBuilder *RegisterAstBuilder(AstBuilder *builder) = 0;

  virtual std::unique_ptr<AstNode> Parse(
      std::vector<Token>::const_iterator begin,
      std::vector<Token>::const_iterator end) const = 0;
};

class Driver {};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_DRIVER_H_