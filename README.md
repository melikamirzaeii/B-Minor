# B-Minor Compiler Front-End & AST Syntax Highlighter (Phase 1)

A modular C++17 front-end for the **B-Minor** programming language implemented strictly with the C++ Standard Library.

## Build & Test
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure