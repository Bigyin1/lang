add_library(lexer lexer.cpp lexFuncs.cpp lexemes.cpp error.cpp)

target_link_libraries(lexer
    PRIVATE project_options project_warnings)

target_include_directories(lexer PUBLIC includes PRIVATE .)

add_custom_target(GenerateLexData
    COMMAND
    go run lexemes/lexemes.go lexemes/genLexData.go
    -tmplDir ./lexemes
    -srcName lexemes.cpp
    -hdrName ./includes/lexer/lexemes.hpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(lexerTest lexer_test.cpp)

target_link_libraries(lexerTest PRIVATE
    lexer project_options project_warnings)

add_test(NAME lexerTest COMMAND lexerTest)
