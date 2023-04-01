#ifndef COWABUNGA_EXAMPLES_PARENTHESESSEQUENCE_PARENTHESESPARSERS_H
#define COWABUNGA_EXAMPLES_PARENTHESESSEQUENCE_PARENTHESESPARSERS_H

#include "cowabunga/Common/IClonableMixin.h"
#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Parser/ICFGRule.h"

enum ParenthesesNonTerminalID {
  NTID_Nested, /// Nested -> () | (Sequence)
  NTID_Sequence /// Sequence -> Nested | Nested Sequence
};

class SoloNestedRule final : public cb::IClonableMixin<cb::ICFGRule, SoloNestedRule> {
public:
  void parse(cb::TokenIterator ItToken) override;

  void produceError(cb::CFGParserError Error) override;

  cb::Symbol getLHSNonTerminal() const override;

  std::vector<cb::Symbol> getProducts() const override;
};

class NestedRule final : public cb::IClonableMixin<cb::ICFGRule, NestedRule> {
public:
  void parse(cb::TokenIterator ItToken) override;

  void produceError(cb::CFGParserError Error) override;

  cb::Symbol getLHSNonTerminal() const override;

  std::vector<cb::Symbol> getProducts() const override;
};

class SoloSequenceRule final : public cb::IClonableMixin<cb::ICFGRule, SoloSequenceRule> {
public:
  void parse(cb::TokenIterator ItToken) override;

  void produceError(cb::CFGParserError Error) override;

  cb::Symbol getLHSNonTerminal() const override;

  std::vector<cb::Symbol> getProducts() const override;
};

class SequenceRule final : public cb::IClonableMixin<cb::ICFGRule, SequenceRule> {
public:
  void parse(cb::TokenIterator ItToken) override;

  void produceError(cb::CFGParserError Error) override;

  cb::Symbol getLHSNonTerminal() const override;

  std::vector<cb::Symbol> getProducts() const override;
};

#endif
