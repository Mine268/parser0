cmake_minimum_required(VERSION 3.16)
project(parser0)

set(CMAKE_CXX_STANDARD 20) # C++ standard should be above 20
set(EXECUTABLE_OUTPUT_PATH bin) # executable file in bin/
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin) # libraries in bin/

include_directories(src/lexer0/src)
add_subdirectory(src/lexer0)
set(lexer_lib token dfa nfa)

add_library(ctx_free_syntax STATIC src/ctx_free_syntax.cpp)

add_executable(main src/main.cpp)
target_link_libraries(main
        ctx_free_syntax
        ${lexer_lib})
