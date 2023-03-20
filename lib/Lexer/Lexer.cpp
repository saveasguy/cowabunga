#include "cowabunga/Lexer/Lexer.h"

#include "cowabunga/Common/Metadata.h"
#include "cowabunga/Lexer/Token.h"

#include <istream>
#include <optional>
#include <string_view>
#include <vector>

using namespace cb;

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

std::optional<std::vector<Token>> Lexer::produceTokens(std::istream &Input) {
  std::vector<Token> Tokens;
  std::string Word;
  std::string_view WordView;
  size_t Offset = 0;
  do {
    if (!WordView.empty()) {
      WordView.remove_prefix(Offset);
    }
    if (WordView.empty()) {
      Word.clear();
      Input >> Word;
      Offset = 0;
      WordView = std::string_view(Word.c_str());
    }
    if (WordView.empty()) {
      return Tokens;
    }
    auto Result = findBestToken(WordView);
    if (!Result.first) {
      return std::nullopt;
    }
    Tokens.push_back(std::move(Result.first));
    Offset = Result.second.length();
    Tokens.back().MetadataStorage.set(Stringified, Result.second);
  } while (Input);
  return Tokens;
}

std::pair<Token, std::string>
Lexer::findBestToken(std::string_view Word) const {
  std::pair<Token, std::string> BestResult;
  for (auto &Tokenizer : Tokenizers) {
    auto Result = Tokenizer->tokenize(Word);
    if (Result > BestResult) {
      BestResult = Result;
    }
  }
  return BestResult;
}
