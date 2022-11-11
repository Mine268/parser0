cmake_minimum_required(VERSION 3.16)
project(parser0)

set(CMAKE_CXX_STANDARD 20) # C++ standard should be above 20
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) # executable file in bin/
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin) # libraries in bin/

include_directories(${PROJECT_SOURCE_DIR}/include) # headers in include/

link_directories(${PROJECT_SOURCE_DIR}/lib) # lexer0 libraries in lib/

add_library(test_lexer STATIC src/test_lexer.cpp) # libraries for test

add_executable(main src/main.cpp) # executable file
target_link_libraries(main
        test_lexer
        # dependencies for lexer0
        nfa
        dfa
        bit_flagger
        token)