#include "cowabunga/Lexer/Lexer.h"

#include "cowabunga/Lexer/Token.h"

#include <cassert>
#include <cctype>
#include <iostream>

using namespace cb;

namespace {

class LineTokenizer final {
public:
  LineTokenizer(std::vector<std::unique_ptr<ITokenizerProxy>> &TokenizersRef,
                std::string FileName);

  std::pair<std::vector<Token>, bool> tokenize(const std::string &LineArg);

private:
  void skipWhitespace();

  void findBestToken();

  void removeNonWhitespaceRange();

  void fillBestTokenMetadata();

  void errorOnUnrecognizedToken() const;

  std::vector<std::unique_ptr<ITokenizerProxy>> &Tokenizers;
  std::shared_ptr<const std::string> File;
  std::shared_ptr<const std::string> SharedLine;
  std::string_view Line;
  Token BestToken;
  size_t BestTokenLength;
  size_t LineNumber;
  size_t Position;
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
  bool Success = true;
  LineTokenizer Tokenizer(Tokenizers, FileName);
  while (std::getline(Input, Line)) {
    auto [NewTokens, LineSuccess] = Tokenizer.tokenize(Line);
    std::move(NewTokens.begin(), NewTokens.end(), std::back_inserter(Tokens));
    Success = Success && LineSuccess;
  }
  if (!Success) {
    std::cerr << "Failed to parse " << FileName << std::endl;
    exit(1);
  }
  return Tokens;
}

LineTokenizer::LineTokenizer(
    std::vector<std::unique_ptr<ITokenizerProxy>> &TokenizersRef,
    std::string FileName)
    : Tokenizers(TokenizersRef),
      File(std::make_shared<const std::string>(std::move(FileName))),
      LineNumber(0), Position(0) {}

std::pair<std::vector<Token>, bool>
LineTokenizer::tokenize(const std::string &LineArg) {
  SharedLine = std::make_shared<const std::string>(LineArg);
  Line = LineArg;
  ++LineNumber;
  Position = 0;
  bool Success = true;
  std::vector<Token> Tokens;
  skipWhitespace();
  while (Position != LineArg.length()) {
    findBestToken();
    if (!BestToken) {
      errorOnUnrecognizedToken();
      removeNonWhitespaceRange();
    } else {
      Line.remove_prefix(BestTokenLength);
      fillBestTokenMetadata();
      Position += BestTokenLength;
      Tokens.push_back(std::move(BestToken));
    }
    skipWhitespace();
  }
  return std::make_pair(Tokens, Success);
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
  std::pair<Token, size_t> BestResult;
  for (auto &Tokenizer : Tokenizers) {
    auto NewResult = Tokenizer->tokenize(Line);
    if (NewResult > BestResult) {
      BestResult = NewResult;
    }
  }
  BestToken = BestResult.first;
  BestTokenLength = BestResult.second;
}

void LineTokenizer::errorOnUnrecognizedToken() const {
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
}

void LineTokenizer::fillBestTokenMetadata() {
  BestToken.Line = SharedLine;
  BestToken.BeginColumnNumber = Position + 1;
  BestToken.EndColumnNumber = Position + 1 + BestTokenLength;
  BestToken.LineNumber = LineNumber;
  BestToken.File = File;
}
