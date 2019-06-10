cmake_minimum_required(VERSION 3.13)
project(gwbasic2c)

set(CMAKE_CXX_STANDARD 17)
find_package(BISON)
find_package(FLEX)

aux_source_directory(gw/core gw_core_source)
aux_source_directory(gw/logic gw_logic_source)
add_library(gw EXCLUDE_FROM_ALL ${gw_core_source} ${gw_logic_source})
target_link_libraries(gw m)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

bison_target(gw_logic_parser gw_logic_parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/gw_logic_parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/gw_logic_parser.h)
flex_target(gw_logic_lexer gw_logic_parser/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/gw_logic_lexer.cpp)
add_flex_bison_dependency(gw_logic_lexer gw_logic_parser)
add_executable(gw_logic_parser EXCLUDE_FROM_ALL gw_logic_parser/main.h gw_logic_parser/main.cpp ${BISON_gw_logic_parser_OUTPUTS} ${FLEX_gw_logic_lexer_OUTPUTS} util.cpp util.h)

file(GLOB gw_core_files gw/core/*.cpp)
file(GLOB gw_logic_files gw/logic/*.cpp)
set(gw_logic_parser_arguments -core ${gw_core_files}
                              -logic ${gw_logic_files}
                              -output ${CMAKE_CURRENT_BINARY_DIR}/gw.h ${CMAKE_CURRENT_BINARY_DIR}/gw.cpp)
add_custom_command(COMMAND gw_logic_parser ${gw_logic_parser_arguments}
                   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gw.h ${CMAKE_CURRENT_BINARY_DIR}/gw.cpp)

bison_target(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h
             VERBOSE REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser-report.txt)
flex_target(lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(lexer parser)
add_executable(gwbasic2c
        main.cpp
        ${BISON_parser_OUTPUTS}
        ${FLEX_lexer_OUTPUTS}
        ${CMAKE_CURRENT_BINARY_DIR}/gw.h ${CMAKE_CURRENT_BINARY_DIR}/gw.cpp
        parser_helper.h parser_helper.cpp
        util.h util.cpp
        ast/node.h ast/node.cpp
        ast/expression.h ast/expression.cpp
        ast/program.cpp ast/program.h)
