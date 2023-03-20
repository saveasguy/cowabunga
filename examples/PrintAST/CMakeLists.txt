add_executable(PrintAST
  PrintAST.cpp
  )
target_link_libraries(PrintAST Common Lexer Parser CBC)
