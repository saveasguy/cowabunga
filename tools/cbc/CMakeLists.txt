add_executable(cbc
  Driver.cpp
  )
target_link_libraries(cbc Common Lexer Parser CBC)
