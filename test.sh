#!/bin/bash

# Compile the test example
echo "Compiling tests..."
clang -o _bin/test src/test_kore.c -std=c23

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo ""
    echo "Running tests..."
    echo ""
    ./_bin/test $*
else
    echo "Compilation failed!"
    exit 1
fi
