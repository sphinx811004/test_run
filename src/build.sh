#!/bin/bash

# Detect architecture
ARCH=$(uname -m)
echo "Detected architecture: $ARCH"

# Define assembly format based on architecture
if [ "$ARCH" = "arm64" ] || [ "$ARCH" = "aarch64" ]; then
    ASM_FORMAT="macho64"
    GCC_ARCH="-arch arm64"
    export ARCH_DEFINE="-DUSE_ARM64=1"
else
    ASM_FORMAT="macho"
    GCC_ARCH=""
    export ARCH_DEFINE=""
fi

echo "Compiling the C++ compiler..."
# Use a different name to avoid conflicts with directories
g++ -std=c++17 -Wall -Wextra $ARCH_DEFINE -o compiler compiler.cpp
if [ $? -ne 0 ]; then
    echo "Error compiling C++ code"
    exit 1
fi

echo "Running the compiler to generate assembly..."
./compiler
if [ $? -ne 0 ]; then
    echo "Error generating assembly"
    exit 1
fi

echo "Assembling the generated code..."
# Use detected format
nasm -f $ASM_FORMAT output.asm -o output.o
if [ $? -ne 0 ]; then
    echo "Error assembling code"
    exit 1
fi

echo "Linking the object file..."
# Use detected architecture flags
gcc $GCC_ARCH output.o -o program
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
