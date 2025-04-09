#!/bin/bash
echo "Compiling the C++ compiler..."
g++ -std=c++17 -Wall -Wextra -o mini_compiler code.cpp
if [ $? -ne 0 ]; then
    echo "Error compiling C++ code"
    exit 1
fi

echo "Running the compiler to generate assembly..."
./mini_compiler
if [ $? -ne 0 ]; then
    echo "Error generating assembly"
    exit 1
fi

echo "Assembling the generated code..."
# Note: macOS uses Mach-O format, not win32
nasm -f macho output.asm -o output.o
if [ $? -ne 0 ]; then
    echo "Error assembling code"
    exit 1
fi

echo "Linking the object file..."
# Note: macOS linking is different from Windows
gcc output.o -o program
if [ $? -ne 0 ]; then
    echo "Error linking"
    exit 1
fi

echo "Running the final program..."
./program
if [ $? -ne 0 ]; then
    echo "Error running program"
    exit 1
fi

echo "Build completed successfully!"
