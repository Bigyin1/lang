add_library(ir_comp
    ir_compile.cpp
)

target_link_libraries(ir_comp
    PRIVATE semantic parser lexer project_options project_warnings)

target_include_directories(ir_comp PUBLIC includes PRIVATE .)

add_executable(ir_comp_test ir_comp_test.cpp)

target_link_libraries(ir_comp_test PRIVATE
    ir_comp semantic parser lexer project_options project_warnings)

add_test(NAME ir_comp_test COMMAND ir_comp_test test.og WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testdata)
