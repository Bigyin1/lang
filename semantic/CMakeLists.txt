add_library(semantic
    scope.cpp
    symtab.cpp
    semantic.cpp
    errors.cpp
)

target_link_libraries(semantic
    PRIVATE parser lexer project_options project_warnings)

target_include_directories(semantic PUBLIC includes PRIVATE .)

add_executable(semanticTest semantic_test.cpp)

target_link_libraries(semanticTest PRIVATE
    semantic parser lexer project_options project_warnings)

add_test(NAME semanticTest COMMAND semanticTest test.og WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testdata)
