add_library(optimizer
    optimizer.cpp
    liveness.cpp
    graphviz.cpp
)

target_link_libraries(optimizer
    PRIVATE inja ir_comp project_options project_warnings)

target_include_directories(optimizer PUBLIC includes PRIVATE .)

# file(COPY ./tmpl DESTINATION ${CMAKE_BINARY_DIR})
add_executable(optimizer_test optimizer_test.cpp)

target_link_libraries(optimizer_test PRIVATE
    inja optimizer ir_comp semantic parser lexer project_options project_warnings)

add_test(NAME optimizer_test COMMAND optimizer_test test.og WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testdata)
