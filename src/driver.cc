#include "driver.h"
#include <map>

#include "lexer.h"

namespace kaleidoc {

AstNode::AstNode(std::map<MetadataType, std::string> metainfo) : metadata_{std::move(metainfo)} {}

void AstNode::AddMetadata(MetadataType type, std::string value) {
  metadata_[type] = value;
}

const std::map<MetadataType, std::string> &AstNode::metadata() const {
  return metadata_;
}

void AstNode::Print(std::ostream &out) const { out << "AST Node"; }

std::ostream &operator<<(std::ostream &out, const AstNode &node) {
  node.Print(out);
  return out;
}

std::map<OperatorId, int> GetOperatorPrecedenceValues() {
  std::map<OperatorId, int> precedence;
  constexpr int kMaxPrecedence = 0xFFFF; // 16 levels of precedence
  precedence[OperatorId::kMinusOp] = kMaxPrecedence >> 1;
  precedence[OperatorId::kPlusOp] = kMaxPrecedence >> 1;
  precedence[OperatorId::kShiftLeftOp] = kMaxPrecedence >> 2;
  precedence[OperatorId::kShiftRightOp] = kMaxPrecedence >> 2;
  precedence[OperatorId::kAssignmentOp] = kMaxPrecedence >> 4;
  return precedence;
}

std::map<OperatorId, OperatorAssociativity> GetOperatorAssociativityValues() {
  std::map<OperatorId, OperatorAssociativity> associativity;
  associativity[OperatorId::kMinusOp] = OperatorAssociativity::kLeftToRight;
  associativity[OperatorId::kPlusOp] = OperatorAssociativity::kLeftToRight;
  associativity[OperatorId::kShiftLeftOp] = OperatorAssociativity::kLeftToRight;
  associativity[OperatorId::kShiftRightOp] = OperatorAssociativity::kLeftToRight;
  associativity[OperatorId::kAssignmentOp] = OperatorAssociativity::kRightToLeft;
  return associativity;
}

}  // namespace kaleidoc
