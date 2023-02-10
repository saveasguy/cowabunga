#include "lexer.h"

#include <cctype>
#include <exception>
#include <istream>
#include <memory>
#include <string>
#include <string_view>

#include "driver.h"
#include "token.h"

namespace kaleidoc {

UnknownLexemeException::UnknownLexemeException(std::string msg)
    : msg_{std::move(msg)} {}

const char *UnknownLexemeException::what() const noexcept {
  return msg_.c_str();
}

FullTextLexer::FullTextLexer(const FullTextLexer &rhs) {
  for (const auto &analyzer : rhs.tokenizers_) {
    tokenizers_.push_back(std::unique_ptr<Tokenizer>{analyzer->Clone()});
  }
}

FullTextLexer &FullTextLexer::operator=(const FullTextLexer &rhs) {
  tokenizers_.clear();
  for (const auto &analyzer : rhs.tokenizers_) {
    tokenizers_.push_back(std::unique_ptr<Tokenizer>{analyzer->Clone()});
  }
  return *this;
}

Lexer &FullTextLexer::AddTokenizer(std::unique_ptr<Tokenizer> tokenizer) {
  tokenizers_.push_back(std::move(tokenizer));
  return *this;
}

std::vector<Token> FullTextLexer::ProduceTokens(std::istream &input) const {
  std::vector<Token> tokens;
  std::string word;
  std::string_view word_view;
  do {
    if (!word_view.empty()) {
      auto offset = tokens.back().stringified().length();
      word_view.remove_prefix(offset);
    }
    if (word_view.empty()) {
      word.clear();
      input >> word;
      word_view = std::string_view{word.c_str()};
    } 
    Token token = FindBestToken(word_view);
    if (!token) {
      throw UnknownLexemeException{
          std::string{word_view.cbegin(), word_view.cend()}};
    }
    tokens.push_back(std::move(token));
  } while (input);
  return tokens;
}

Token FullTextLexer::FindBestToken(std::string_view word) const {
  Token best_token;
  for (const auto &tokenizer : tokenizers_) {
    Token cur_token = tokenizer->Tokenize(word);
    if (cur_token > best_token) { best_token = cur_token; }
  }
  return best_token;
}

std::unique_ptr<Lexer> FullTextLexer::Clone() const {
  return std::make_unique<FullTextLexer>(*this);
}

}  // namespace kaleidoc
