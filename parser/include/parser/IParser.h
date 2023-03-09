#ifndef PARSER_IPARSER_H
#define PARSER_IPARSER_H

#include <vector>

namespace cb {

namespace common {

class Token;

} // namespace common

namespace parser {

using TokenIterator = std::vector<common::Token>::const_iterator;

template <class TProduct>
class IParser {
public:
  virtual TProduct parse(TokenIterator ItBegin, TokenIterator ItEnd) = 0;

  virtual ~IParser() = default;
};

} // namespace parser

} // namespace cb

#endif
