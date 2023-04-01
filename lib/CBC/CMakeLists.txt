add_library(CBC
  ASTBuilder.cpp
  ASTNodes.cpp
  ASTPasses.cpp
  Parsers.cpp
  Tokenizers.cpp
)
target_link_libraries(CBC Common Lexer Parser)
