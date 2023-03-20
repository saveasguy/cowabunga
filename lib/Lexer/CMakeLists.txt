add_library(Lexer
  ITokenizer.cpp
  Lexer.cpp
  Token.cpp
  TokenizerProxy.cpp
  )
target_link_libraries(Lexer Common)
