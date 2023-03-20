add_library(CBC
  ASTNodes.cpp
  ASTPasses.cpp
  Definitions.cpp
  Parsers.cpp
  Tokenizers.cpp)
target_link_libraries(CBC Common Lexer Parser)
