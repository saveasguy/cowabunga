#include "cowabunga/CBC/Tokenizers.h"
#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Parser/CFGParser.h"
#include "cowabunga/Parser/Symbol.h"

#include "ParenthesesParsers.h"

#include <memory>
#include <sstream>


int main() {
  cb::Lexer Lex;
  Lex.addTokenizer(cb::KeywordTokenizer(cb::TID_OpenParantheses, "("))
  .addTokenizer(cb::KeywordTokenizer(cb::TID_CloseParantheses, ")"));
  std::stringstream Stream;
  Stream << "(())()";
  auto Tokens = Lex.tokenize(Stream, "stringstream");
  cb::CFGParser Parser(cb::Symbol(NTID_Sequence, false));
  Parser.addCFGRule(SoloSequenceRule());
  Parser.addCFGRule(SoloNestedRule());
  Parser.addCFGRule(NestedRule());
  Parser.addCFGRule(SequenceRule());
  Parser.parse(Tokens.cbegin(), Tokens.cend());
  return 0;
}