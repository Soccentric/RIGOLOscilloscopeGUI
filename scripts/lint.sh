#!/bin/bash

# Run cpplint on all C++ source and header files

find src -name "*.cpp" -o -name "*.h" | xargs cpplint

echo "Linting completed."