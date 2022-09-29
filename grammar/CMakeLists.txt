cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(smithscript_grammar)

include_directories(${PROJECT_SOURCE_DIR}/../src)
include_directories(${PROJECT_SOURCE_DIR}/../lib/compile-time-regular-expressions/single-header)

add_executable(${PROJECT_NAME} grammar.cpp)