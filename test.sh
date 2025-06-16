#!/bin/bash

# Compile the test example
echo "Compiling test example..."
clang -o _bin/test_example src/test_kore.c -std=c23

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo ""
    echo "Running tests..."
    echo ""
    ./_bin/test_example
else
    echo "Compilation failed!"
    exit 1
fi
