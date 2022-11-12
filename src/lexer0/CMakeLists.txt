cmake_minimum_required(VERSION 3.16)
project(lexer0)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)

add_library(bit_flagger STATIC src/bit_flagger.cpp)

add_library(dfa STATIC src/dfa.cpp)
target_link_libraries(dfa bit_flagger)

add_library(nfa STATIC src/nfa.cpp)
target_link_libraries(nfa bit_flagger dfa)

add_library(reg_expr STATIC src/reg_expr.cpp)
target_link_libraries(reg_expr nfa dfa)

add_library(token STATIC src/token.cpp)