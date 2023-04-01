add_library(CBC
  ASTBuilder.cpp
  ASTNodes.cpp
  ASTPasses.cpp
  Parsers.cpp
  Tokenizers.cpp
)
execute_process(COMMAND llvm-config --libs OUTPUT_VARIABLE LLVM_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
target_link_libraries(CBC Common Lexer Parser ${LLVM_LIB})
