#ifndef COWABUNGA_LEXER_ITOKENIZER_H
#define COWABUNGA_LEXER_ITOKENIZER_H

#include <string_view>
#include <type_traits>

namespace cb {

class Token;

class ITokenizer {
public:
  virtual std::pair<Token, std::string> tokenize(std::string_view Word) = 0;

  virtual ~ITokenizer();
};

} // namespace cb

#endif // COWABUNGA_LEXER_ITOKENIZER_H
