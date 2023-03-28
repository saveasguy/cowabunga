add_executable(PrintTokens
  PrintTokens.cpp
)
target_link_libraries(PrintTokens CBC Common Lexer Parser)
