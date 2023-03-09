#include "common/Metadata.h"

#include <cassert>

namespace cb {

namespace common {

void Metadata::set(MetadataType Type, std::string Value) {
  DataStorage[Type] = std::move(Value);
}

std::string Metadata::get(MetadataType Type) const {
  auto It = DataStorage.find(Type);
  assert(It != DataStorage.cend() && "Couldn't extract metadata!");
  return It->second;
}

} // namespace common

} // namespace cb
