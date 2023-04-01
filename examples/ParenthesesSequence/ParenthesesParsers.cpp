#include "ParenthesesParsers.h"
#include "cowabunga/CBC/Tokenizers.h"
#include "cowabunga/Parser/Symbol.h"

#include <iostream>

void SoloNestedRule::parse(cb::TokenIterator ItToken) { std::cout << "Nested ::= ()\n"; }

void SoloNestedRule::produceError(cb::CFGParserError Error) {
  std::cout << "Failed to parse\n";
}

cb::Symbol SoloNestedRule::getLHSNonTerminal() const {
  return cb::Symbol(NTID_Nested, false);
}

std::vector<cb::Symbol> SoloNestedRule::getProducts() const {
  return {cb::Symbol(cb::TID_OpenParantheses),
          cb::Symbol(cb::TID_CloseParantheses)};
}

void NestedRule::parse(cb::TokenIterator ItToken) { std::cout << "Nested ::= (Sequence)\n"; }

void NestedRule::produceError(cb::CFGParserError Error) {
  std::cout << "Failed to parse\n";
}

cb::Symbol NestedRule::getLHSNonTerminal() const {
  return cb::Symbol(NTID_Nested, false);
}

std::vector<cb::Symbol> NestedRule::getProducts() const {
  return {cb::Symbol(cb::TID_OpenParantheses), cb::Symbol(NTID_Sequence, false),
          cb::Symbol(cb::TID_CloseParantheses)};
}

void SoloSequenceRule::parse(cb::TokenIterator ItToken) { std::cout << "Sequence ::= Nested\n"; }

void SoloSequenceRule::produceError(cb::CFGParserError Error) {
  std::cout << "Failed to parse\n";
}

cb::Symbol SoloSequenceRule::getLHSNonTerminal() const {
  return cb::Symbol(NTID_Sequence, false);
}

std::vector<cb::Symbol> SoloSequenceRule::getProducts() const {
  return {cb::Symbol(NTID_Nested, false)};
}

void SequenceRule::parse(cb::TokenIterator ItToken) { std::cout << "Sequence ::= Nested Sequence\n"; }

void SequenceRule::produceError(cb::CFGParserError Error) {
  std::cout << "Failed to parse\n";
}

cb::Symbol SequenceRule::getLHSNonTerminal() const {
  return cb::Symbol(NTID_Sequence, false);
}

std::vector<cb::Symbol> SequenceRule::getProducts() const {
  return {cb::Symbol(NTID_Nested, false), cb::Symbol(NTID_Sequence, false)};
}