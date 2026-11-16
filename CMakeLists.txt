cmake_minimum_required(VERSION 3.16)
project(bminor_cc VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(CORE_SOURCES
    src/common/Diagnostics.cpp
    src/lexer/Token.cpp
    src/lexer/Lexer.cpp
    src/ast/AST.cpp
    src/ast/ASTPrinter.cpp
    src/parser/Parser.cpp
    src/highlighter/SyntaxHighlighter.cpp
    src/highlighter/ANSIRenderer.cpp
    src/highlighter/HTMLRenderer.cpp
)

add_library(bminor_core STATIC ${CORE_SOURCES})

add_executable(bminor_cc src/main.cpp)
target_link_libraries(bminor_cc PRIVATE bminor_core)

enable_testing()
add_executable(bminor_tests tests/test_runner.cpp)
target_link_libraries(bminor_tests PRIVATE bminor_core)
add_test(NAME BMinorPhase1Tests COMMAND bminor_tests ${PROJECT_SOURCE_DIR}/tests)