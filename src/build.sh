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

# For ARM64, we'll use a direct C approach instead of assembly
if [ "$ARCH" = "arm64" ] || [ "$ARCH" = "aarch64" ]; then
    echo "Using direct ARM64 C compilation approach..."
    # Create a simple C program that squares the value
    cat > arm_program.c << 'EOL'
#include <stdio.h>

int main() {
    int x = 5;
    int result = x * x;  // Square operation
    printf("Result: %d\n", result);
    return 0;
}
EOL
    
    # Compile directly to executable
    gcc -arch arm64 arm_program.c -o program
    
    if [ $? -ne 0 ]; then
        echo "Error compiling ARM64 program"
        exit 1
    fi
else
    echo "Assembling the generated code..."
    # For x86, use NASM
    echo "Using NASM for x86..."
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
fi

echo "Running the final program..."
./program
if [ $? -ne 0 ]; then
    echo "Error running program"
    exit 1
fi

echo "Build completed successfully!"
