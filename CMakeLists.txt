cmake_minimum_required(VERSION 3.13)
project(gwbasic2c)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

find_package(BISON)
find_package(FLEX)
bison_target(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h)
flex_target(lexer lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(lexer parser)

add_executable(gwbasic2c main.cpp ast.cpp ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS})