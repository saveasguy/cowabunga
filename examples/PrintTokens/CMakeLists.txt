add_executable(PrintTokens
  PrintTokens.cpp
  )
target_link_libraries(PrintTokens Common Lexer Parser CBC)
