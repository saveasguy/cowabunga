#include "lexer.h"

#include <cctype>
#include <cstdio>
#include <exception>
#include <istream>
#include <memory>
#include <string>

#include "driver.h"
#include "token.h"

namespace kaleidoc {

UnknownLexemeException::UnknownLexemeException(const std::string &msg)
    : msg_{ msg } {}

const char *UnknownLexemeException::what() const noexcept {
  return msg_.c_str();
}

SymbolicLexer::SymbolicLexer(const SymbolicLexer &rhs) {
  for (const auto &analyzer : rhs.analyzers_) {
    analyzers_.push_back(std::unique_ptr<LexemeAnalyzer>{ analyzer->Clone() });
  }
}

SymbolicLexer &SymbolicLexer::operator=(const SymbolicLexer &rhs) {
  analyzers_.clear();
  for (const auto &analyzer : rhs.analyzers_) {
    analyzers_.push_back(std::unique_ptr<LexemeAnalyzer>{ analyzer->Clone() });
  }
  return *this;
}

Lexer *SymbolicLexer::AddAnalyzer(const LexemeAnalyzer *analyzer) {
  analyzers_.push_back(std::unique_ptr<LexemeAnalyzer>{ analyzer->Clone() });
  return this;
}

Token *SymbolicLexer::NextToken(std::istream &input) {
  if (!input) { return new EofToken; }
  SkipBlankSpace(input);
  return GetBestMatchedToken(input);
}

void SymbolicLexer::SkipBlankSpace(std::istream &input) {
  char current_char;
  do {
    input.get(current_char);
  } while (input && std::isspace(current_char) != 0);
  input.unget();
}

Token *SymbolicLexer::GetBestMatchedToken(std::istream &input) {
  LexemeAnalyzer *matching_analyzer = nullptr;
  TokenPriority max_priority = TokenPriority::kUnmatched;
  char first_char = ' ';
  input.get(first_char);
  std::string current_word = { first_char };
  for (auto &analyzer : analyzers_) {
    analyzer->Flush();
    TokenPriority current_priority = analyzer->CheckWholeWord(current_word);
    if (current_priority == TokenPriority::kUnmatched) { continue; }
    if (current_priority > max_priority) {
      matching_analyzer = analyzer.get();
      max_priority = current_priority;
    }
    std::string word_tail = TryGetWordTail(input, analyzer.get());
    if (!word_tail.empty()) {
      current_word += word_tail;
      matching_analyzer = analyzer.get();
    }
  }
  if (!matching_analyzer) {
    throw UnknownLexemeException{ "Unknown lexeme: " + current_word };
  }
  return matching_analyzer->GetToken(current_word);
}

std::string SymbolicLexer::TryGetWordTail(std::istream &input,
                                  LexemeAnalyzer *analyzer) {
  char current_char;
  std::string current_word;
  while (input.get(current_char) &&
         analyzer->CheckNextChar(current_char) != TokenPriority::kUnmatched) {
    current_word += current_char;
  }
  input.unget();
  return current_word;
}

Lexer *SymbolicLexer::Clone() const {
  std::unique_ptr<Lexer> cloned{ new SymbolicLexer };
  for (const auto &analyzer : analyzers_) {
    cloned->AddAnalyzer(analyzer->Clone());
  }
  return cloned.release();
}

}  // namespace kaleidoc
