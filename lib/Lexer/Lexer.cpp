#include "cowabunga/Lexer/Lexer.h"

#include "cowabunga/Lexer/Token.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <optional>

using namespace cb;

namespace {

class LineTokenizer final {
public:
  LineTokenizer(std::vector<std::unique_ptr<ITokenizerProxy>> &TokenizersRef,
                std::string FileName);

  std::vector<Token> tokenize(const std::string &LineArg);

  int getError() const noexcept;

  operator bool() const noexcept;

private:
  void skipWhitespace();

  void findBestToken();

  void removeNonWhitespaceRange();

  void fillBestTokenMetadata();

  void raiseErrorOnUnrecognizedToken();

  std::vector<std::unique_ptr<ITokenizerProxy>> &Tokenizers;
  std::shared_ptr<const std::string> File;
  std::shared_ptr<const std::string> SharedLine;
  std::string_view Line;
  std::optional<Token> BestToken;
  size_t BestTokenLength;
  size_t LineNumber;
  size_t Position;
  int Error;
};

} // namespace

Lexer::Lexer(const Lexer &RHS) {
  Tokenizers.reserve(RHS.Tokenizers.size());
  for (auto &Tokenizer : RHS.Tokenizers) {
    Tokenizers.push_back(Tokenizer->clone());
  }
}

Lexer &Lexer::operator=(const Lexer &RHS) {
  Lexer NewLexer(RHS);
  std::swap(*this, NewLexer);
  return *this;
}

std::vector<Token> Lexer::tokenize(std::istream &Input,
                                   const std::string &FileName) {
  std::vector<Token> Tokens;
  std::string Line;
  LineTokenizer Tokenizer(Tokenizers, FileName);
  while (std::getline(Input, Line)) {
    auto NewTokens = Tokenizer.tokenize(Line);
    std::move(NewTokens.begin(), NewTokens.end(), std::back_inserter(Tokens));
  }
  if (!Tokenizer) {
    std::cerr << "Failed to parse " << FileName << std::endl;
    exit(1);
  }
  return Tokens;
}

std::string Lexer::getTokenLexeme(int ID) const {
  return LexemeStringMapping.at(ID);
}

LineTokenizer::LineTokenizer(
    std::vector<std::unique_ptr<ITokenizerProxy>> &TokenizersRef,
    std::string FileName)
    : Tokenizers(TokenizersRef),
      File(std::make_shared<const std::string>(std::move(FileName))),
      LineNumber(0), Position(0), Error(0) {}

std::vector<Token> LineTokenizer::tokenize(const std::string &LineArg) {
  SharedLine = std::make_shared<const std::string>(LineArg);
  Line = LineArg;
  ++LineNumber;
  Position = 0;
  std::vector<Token> Tokens;
  skipWhitespace();
  while (Position != LineArg.length()) {
    findBestToken();
    if (!BestToken) {
      raiseErrorOnUnrecognizedToken();
      removeNonWhitespaceRange();
    } else {
      Line.remove_prefix(BestTokenLength);
      fillBestTokenMetadata();
      Position += BestTokenLength;
      Tokens.push_back(std::move(*BestToken));
    }
    skipWhitespace();
  }
  return Tokens;
}

void LineTokenizer::skipWhitespace() {
  size_t WhitespacePrefixLength = Line.find_first_not_of(" \t");
  if (WhitespacePrefixLength != std::string_view::npos) {
    Line.remove_prefix(WhitespacePrefixLength);
    Position += WhitespacePrefixLength;
  } else {
    Position += Line.length();
  }
}

void LineTokenizer::removeNonWhitespaceRange() {
  size_t NonWhitespaceLength = Line.find_first_of(" \t");
  if (NonWhitespaceLength == std::string_view::npos) {
    NonWhitespaceLength = Line.length();
  }
  Line.remove_prefix(NonWhitespaceLength);
  Position += NonWhitespaceLength;
}

void LineTokenizer::findBestToken() {
  std::optional<Token> BestFoundToken;
  size_t BestFoundTokenLength = 0;
  for (auto &Tokenizer : Tokenizers) {
    auto [FoundToken, TokenLength] = Tokenizer->tokenize(Line);
    if (BestFoundToken < FoundToken) {
      BestFoundToken = FoundToken;
      BestFoundTokenLength = TokenLength;
    }
  }
  BestToken = BestFoundToken;
  BestTokenLength = BestFoundTokenLength;
}

void LineTokenizer::raiseErrorOnUnrecognizedToken() {
  assert(SharedLine && "SharedLine shouldn't be nullptr");
  assert(Position < SharedLine->length() &&
         "Position should be less than line's length");
  std::cerr << *File << ":" << LineNumber << ":" << Position + 1
            << ": unrecognized lexeme\n";
  std::cerr << "\t" << *SharedLine << "\n\t";
  for (size_t I = 0; I < Position; ++I) {
    std::cerr << " ";
  }
  std::cerr << "^";
  for (auto *It = Line.cbegin() + 1, *ItEnd = Line.cend();
       It != ItEnd && !std::isspace(*It); ++It) {
    std::cerr << "~";
  }
  std::cerr << "\n";
  Error = 1;
}

void LineTokenizer::fillBestTokenMetadata() {
  assert(
      BestToken.has_value() &&
      "LineTokenizer's BestToken member is nullopt, but has to contain value");
  BestToken->Line = SharedLine;
  BestToken->BeginColumnNumber = Position + 1;
  BestToken->EndColumnNumber = Position + 1 + BestTokenLength;
  BestToken->LineNumber = LineNumber;
  BestToken->File = File;
}

int LineTokenizer::getError() const noexcept { return Error; }

LineTokenizer::operator bool() const noexcept { return Error == 0; }
