#!/bin/bash

# Format all C++ source and header files using clang-format

find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i

echo "Code formatting completed."