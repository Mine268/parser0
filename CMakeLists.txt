cmake_minimum_required(VERSION 3.16)
project(parser0)

set(CMAKE_CXX_STANDARD 20) # C++ standard should be above 20
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) # executable file in bin/
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin) # libraries in bin/

include_directories(src/lexer0/src)
add_subdirectory(src/lexer0)
set(lexer_lib token dfa nfa)

add_executable(main src/main.cpp)
target_link_libraries(main ${lexer_lib})
