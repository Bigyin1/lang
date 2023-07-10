add_library(parser parser.cpp parse_expr.cpp nodes.cpp errors.cpp)

target_link_libraries(parser
    PRIVATE lexer project_options project_warnings)

target_include_directories(parser PUBLIC includes PRIVATE .)
