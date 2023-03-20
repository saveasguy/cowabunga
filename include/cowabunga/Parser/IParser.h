#ifndef COWABUNGA_PARSER_IPARSER_H
#define COWABUNGA_PARSER_IPARSER_H

#include <vector>

namespace cb {

class Token;

using TokenIterator = std::vector<Token>::const_iterator;

template <class TProduct> class IParser {
public:
  virtual TProduct parse(TokenIterator ItBegin, TokenIterator ItEnd) = 0;

  virtual ~IParser() = default;
};

} // namespace cb

#endif // COWABUNGA_PARSER_IPARSER_H
