add_executable(ParenthesesSequence
  ParenthesesParsers.cpp
  ParenthesesSequence.cpp
)
target_link_libraries(ParenthesesSequence CBC Common Lexer Parser)
