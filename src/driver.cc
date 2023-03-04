#include "driver.h"

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

}  // namespace kaleidoc
