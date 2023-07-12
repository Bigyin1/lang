add_library(parser
    parser.cpp
    parse_expr.cpp
    parse_stmt.cpp
    parse_programm.cpp
    nodes.cpp
    errors.cpp
    grVizGen.cpp
)

target_link_libraries(parser
    PRIVATE lexer project_options project_warnings)

target_include_directories(parser PUBLIC includes PRIVATE .)

add_executable(parserTest parser_test.cpp)

target_link_libraries(parserTest PRIVATE
    parser lexer project_options project_warnings)

add_test(NAME parserTest COMMAND parserTest test.og tree.dot WORKING_DIRECTORY testdata)
