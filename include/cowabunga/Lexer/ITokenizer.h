#ifndef COWABUNGA_LEXER_ITOKENIZER_H
#define COWABUNGA_LEXER_ITOKENIZER_H

#include <optional>
#include <string_view>
#include <type_traits>

namespace cb {

class Token;

class ITokenizer {
public:
  virtual std::pair<std::optional<Token>, size_t> tokenize(std::string_view Word) = 0;

  virtual int getTokenID() const = 0;

  virtual std::string getLexemeString() const = 0;

  virtual ~ITokenizer();
};

} // namespace cb

#endif // COWABUNGA_LEXER_ITOKENIZER_H
