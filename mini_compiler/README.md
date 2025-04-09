# Mini Compiler

A simple compiler that demonstrates basic compilation concepts, including lexing, parsing, and code generation. This compiler supports a minimal language with basic arithmetic operations and a custom square operation.

## Quick Start

The easiest way to get started is to run the automatic installation script:

```bash
.\install_requirements.bat
```

This script will:
1. Download and install NASM
2. Download and install MinGW-w64
3. Set up the PATH environment variables
4. Verify the installations

After running the script, restart your terminal and you're ready to go!

## Manual Installation

If you prefer to install the tools manually:

### Prerequisites

1. **NASM (Netwide Assembler)**
   - Download from: https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/
   - Extract to: `C:\nasm`
   - Add to PATH: `C:\nasm`

2. **MinGW-w64 (GCC for Windows)**
   - Download from: https://github.com/niXman/mingw-builds-binaries/releases
   - Extract to: `C:\mingw64`
   - Add to PATH: `C:\mingw64\bin`

### Setting Up Environment

After installing the prerequisites, run:
```bash
.\setup_path.bat
```

This script will:
- Verify NASM installation
- Verify GCC installation
- Help you add them to PATH if needed

## Building and Running

To compile and run the mini compiler:

```bash
.\build.bat
```

This script will:
1. Compile the C++ compiler
2. Generate assembly code
3. Assemble with NASM
4. Link with GCC
5. Run the program

## Example Usage

The compiler supports simple expressions like:
```
x = 5 square
```

This will:
1. Parse the expression
2. Generate x86 assembly code
3. Compile and run the code
4. Output: `Result: 25`

## Generated Assembly Code

The compiler generates x86 assembly code with a custom square macro:

```nasm
; Square operation macro
%macro square 1
    ; Save operand
    mov ebx, %1
    ; Square the value
    imul %1, ebx
%endmacro
```

## Troubleshooting

1. **"Command not found" errors**
   - Run `.\setup_path.bat` to verify PATH settings
   - Restart your terminal after installation

2. **NASM/GCC not found**
   - Verify installations using `nasm --version` and `gcc --version`
   - Run `.\install_requirements.bat` to reinstall if needed

3. **Permission errors**
   - Run Command Prompt as Administrator
   - Ensure you have write permissions to `C:\nasm` and `C:\mingw64`

## Project Structure

- `code.cpp` - Main compiler implementation
- `build.bat` - Build and run script
- `setup_path.bat` - Environment setup script
- `install_requirements.bat` - Automatic installation script
- `output.asm` - Generated assembly code
- `program.exe` - Final executable

## License

This project is open source under the MIT License. 