cmake_minimum_required(VERSION 3.17)
project(Lang VERSION 0.1 LANGUAGES CXX)

include(cmake/CompileOptions.cmake)

enable_testing()

add_subdirectory(lexer)
