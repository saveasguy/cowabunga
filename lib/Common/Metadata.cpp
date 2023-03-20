#include "cowabunga/Common/Metadata.h"

#include <cassert>

using namespace cb;

void Metadata::set(MetadataType Type, std::string Value) {
  DataStorage[Type] = std::move(Value);
}

std::string Metadata::get(MetadataType Type) const {
  auto It = DataStorage.find(Type);
  assert(It != DataStorage.cend() && "Couldn't extract metadata!");
  return It->second;
}
