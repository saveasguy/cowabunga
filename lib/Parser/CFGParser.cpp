#include "cowabunga/Parser/CFGParser.h"

#include "cowabunga/Common/IClonableMixin.h"
#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Parser/CFGParserError.h"
#include "cowabunga/Parser/ICFGRule.h"
#include "cowabunga/Parser/Symbol.h"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

using namespace cb;

namespace {

class ParserNode final {
public:
  ParserNode(Symbol StartSymbol, TokenIterator ItInputBegin,
             TokenIterator ItInputEnd);

  bool parseFrontTerminals();

  bool isSuccessful() const noexcept;

  bool checkStackEmpty() const noexcept;

  bool checkInputEmpty() const noexcept;

  Symbol getTopSymbol() const;

  Symbol getTopNonTerminal() const;

  Symbol getTopTerminal() const;

  ICFGRule *getLatestUsedRule() noexcept;

  std::vector<std::pair<ICFGRule *, TokenIterator>> getParsingTrace();

  void applyRule(ICFGRule *Rule);

  TokenIterator getInputIterator() const noexcept;

private:
  void updateRuleStack();

  TokenIterator ItInput;
  TokenIterator ItEnd;
  std::vector<std::pair<ICFGRule *, TokenIterator>> Trace;
  std::stack<std::pair<ICFGRule *, size_t>> RuleStack;
  std::stack<Symbol> SymbolStack;
  ICFGRule *LatestUsedRule;
};

class CFGParserImpl final {
public:
  using CFGRuleIterator = std::vector<std::unique_ptr<ICFGRule>>::iterator;

  CFGParserImpl(Symbol StartSymbol,
                std::vector<std::unique_ptr<ICFGRule>> &CFGRules);

  void parse(TokenIterator ItBegin, TokenIterator ItEnd);

private:
  void parsingLoop();

  void prepareError(ParserNode &Node);

  void finishParsing(ParserNode &Node);

  void extendParsingTree(const ParserNode &Node);

  void finishParsingOrMakeError(std::shared_ptr<ParserNode> Node);

  std::pair<CFGRuleIterator, CFGRuleIterator>
  findRulesForNonTerminal(Symbol NonTerminal) const;

  static bool compareByNonTerminalID(const std::unique_ptr<ICFGRule> &LHS,
                                     int RHS);

  CFGRuleIterator findFirstRuleForNonTerminalID(
      int NonTerminalID,
      bool (*Less)(const std::unique_ptr<ICFGRule> &LHS, int RHS)) const;

  std::stack<ParserNode> ParsingTreeLeaves;
  CFGParserError Error;
  Symbol Start;
  std::vector<std::unique_ptr<ICFGRule>> &Rules;
  bool Success;
};

} // namespace

CFGParser::CFGParser(Symbol StartNonTerminal)
    : StartSymbol(std::move(StartNonTerminal)) {}

CFGParser::CFGParser(const CFGParser &RHS) : StartSymbol(RHS.StartSymbol) {
  Rules.reserve(RHS.Rules.size());
  for (auto &Rule : Rules) {
    Rules.push_back(Rule->clone());
  }
}

CFGParser &CFGParser::operator=(const CFGParser &RHS) {
  CFGParser NewParser(RHS);
  std::swap(*this, NewParser);
  return *this;
}

void CFGParser::parse(TokenIterator ItBegin, TokenIterator ItEnd) {
  if (ItBegin == ItEnd) {
    return;
  }
  CFGParserImpl Impl(StartSymbol, Rules);
  Impl.parse(ItBegin, ItEnd);
}

CFGParserImpl::CFGParserImpl(Symbol StartSymbol,
                             std::vector<std::unique_ptr<ICFGRule>> &CFGRules)
    : Start(StartSymbol), Rules(CFGRules) {}

void CFGParserImpl::parse(TokenIterator ItBegin, TokenIterator ItEnd) {
  Error.ItFoundToken = ItBegin;
  Success = false;
  ParsingTreeLeaves.push(ParserNode(Start, ItBegin, ItEnd));
  parsingLoop();
  if (!Success) {
    Error.FailedRule->produceError(Error);
  }
}

void CFGParserImpl::parsingLoop() {
  while (!ParsingTreeLeaves.empty()) {
    auto Leaf = ParsingTreeLeaves.top();
    ParsingTreeLeaves.pop();
    bool Matched = Leaf.parseFrontTerminals();
    if (!Matched) {
      prepareError(Leaf);
      continue;
    }
    if (Leaf.isSuccessful()) {
      finishParsing(Leaf);
    } else {
      extendParsingTree(Leaf);
    }
  }
}

void CFGParserImpl::prepareError(ParserNode &Node) {
  if (Node.getInputIterator() - Error.ItFoundToken < 0) {
    return;
  }
  Error.EOFExpected = Node.checkStackEmpty();
  Error.EOFFound = Node.checkInputEmpty();
  Error.ItFoundToken = Node.getInputIterator();
  if (Error.EOFFound) {
    --Error.ItFoundToken;
  }
  if (!Error.EOFExpected) {
    Error.ExpectedSymbol = Node.getTopSymbol();
  }
  Error.FailedRule = Node.getLatestUsedRule();
}

void CFGParserImpl::finishParsing(ParserNode &Node) {
  Success = true;
  auto Trace = Node.getParsingTrace();
  auto It = Trace.rbegin();
  auto ItEnd = Trace.rend();
  for (; It != ItEnd; ++It) {
    It->first->parse(It->second);
  }
  while (!ParsingTreeLeaves.empty()) {
    ParsingTreeLeaves.pop();
  }
}

void CFGParserImpl::extendParsingTree(const ParserNode &Node) {
  auto NonTerminal = Node.getTopNonTerminal();
  auto [ItBegin, ItEnd] = findRulesForNonTerminal(NonTerminal);
  for (; ItBegin != ItEnd; ++ItBegin) {
    auto NewNode = Node;
    NewNode.applyRule(ItBegin->get());
    ParsingTreeLeaves.push(std::move(NewNode));
  }
}

std::pair<CFGParserImpl::CFGRuleIterator, CFGParserImpl::CFGRuleIterator>
CFGParserImpl::findRulesForNonTerminal(Symbol NonTerminal) const {
  int NonTerminalID = NonTerminal.getID();
  auto ItBegin =
      findFirstRuleForNonTerminalID(NonTerminalID, compareByNonTerminalID);
  auto ItEnd =
      findFirstRuleForNonTerminalID(NonTerminalID + 1, compareByNonTerminalID);
  return std::make_pair(ItBegin, ItEnd);
}

bool CFGParserImpl::compareByNonTerminalID(const std::unique_ptr<ICFGRule> &LHS,
                                           int RHS) {
  return LHS->getLHSNonTerminal().getID() < RHS;
}

CFGParserImpl::CFGRuleIterator CFGParserImpl::findFirstRuleForNonTerminalID(
    int NonTerminalID,
    bool (*Less)(const std::unique_ptr<ICFGRule> &LHS, int RHS)) const {
  auto Distance = Rules.size();
  CFGRuleIterator ItBegin = Rules.begin();
  CFGRuleIterator It;
  while (Distance > 0) {
    auto Step = Distance / 2;
    It = ItBegin + Step;
    if (Less(*It, NonTerminalID)) {
      ItBegin = ++It;
      Distance -= Step + 1;
    } else {
      Distance = Step;
    }
  }
  return ItBegin;
}

ParserNode::ParserNode(Symbol StartSymbol, TokenIterator ItInputBegin,
                       TokenIterator ItInputEnd)
    : ItInput(ItInputBegin), ItEnd(ItInputEnd), LatestUsedRule(nullptr) {
  SymbolStack.push(StartSymbol);
  RuleStack.push(std::make_pair(nullptr, 1));
}

bool ParserNode::parseFrontTerminals() {
  assert(!SymbolStack.empty() && "Parsing is already completed");
  for (; ItInput != ItEnd && !SymbolStack.empty() &&
         SymbolStack.top().isTerminal();
       ++ItInput) {
    if (SymbolStack.top().getID() != ItInput->getID()) {
      return false;
    }
    SymbolStack.pop();
    updateRuleStack();
  }
  return checkInputEmpty() && checkStackEmpty() ||
         !checkInputEmpty() && !checkStackEmpty();
}

bool ParserNode::isSuccessful() const noexcept {
  return checkInputEmpty() && checkStackEmpty();
}

bool ParserNode::checkStackEmpty() const noexcept {
  return SymbolStack.empty();
}

bool ParserNode::checkInputEmpty() const noexcept { return ItInput == ItEnd; }

void ParserNode::updateRuleStack() {
  assert(!RuleStack.empty() && "RuleStack shouldn't be empty");
  assert(RuleStack.top().second != 0 && "Rule is already applied");
  --RuleStack.top().second;
  LatestUsedRule = RuleStack.top().first;
  if (RuleStack.top().second == 0) {
    RuleStack.pop();
  }
}

Symbol ParserNode::getTopSymbol() const {
  assert(!SymbolStack.empty() && "Parsing is already completed");
  return SymbolStack.top();
}

Symbol ParserNode::getTopNonTerminal() const {
  assert(!SymbolStack.empty() && "Parsing is already completed");
  assert(SymbolStack.top().isNonTerminal() &&
         "Top Symbol should be nonterminal");
  return SymbolStack.top();
}

Symbol ParserNode::getTopTerminal() const {
  assert(!SymbolStack.empty() && "Parsing is already completed");
  assert(SymbolStack.top().isTerminal() && "Top Symbol should be terminal");
  return SymbolStack.top();
}

ICFGRule *ParserNode::getLatestUsedRule() noexcept {
  assert(LatestUsedRule && "At least one Rule should be used");
  return LatestUsedRule;
}

std::vector<std::pair<ICFGRule *, TokenIterator>>
ParserNode::getParsingTrace() {
  return Trace;
}

void ParserNode::applyRule(ICFGRule *Rule) {
  assert(!SymbolStack.empty() && "Parsing is already completed");
  assert(SymbolStack.top().isNonTerminal() &&
         "Top Symbol should be nonterminal");
  SymbolStack.pop();
  updateRuleStack();
  Trace.push_back(std::make_pair(Rule, ItInput));
  auto Products = Rule->getProducts();
  RuleStack.push(std::make_pair(Rule, Products.size()));
  for (auto It = Products.rbegin(), ItEnd = Products.rend(); It != ItEnd;
       ++It) {
    SymbolStack.push(*It);
  }
}

TokenIterator ParserNode::getInputIterator() const noexcept { return ItInput; }
