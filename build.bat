@echo off
echo Compiling the C++ compiler...
g++ -std=c++17 -Wall -Wextra -o mini_compiler code.cpp
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling C++ code
    pause
    exit /b %ERRORLEVEL%
)

echo Running the compiler to generate assembly...
mini_compiler
if %ERRORLEVEL% NEQ 0 (
    echo Error generating assembly
    pause
    exit /b %ERRORLEVEL%
)

echo Assembling the generated code...
nasm -f win32 output.asm -o output.obj
if %ERRORLEVEL% NEQ 0 (
    echo Error assembling code
    pause
    exit /b %ERRORLEVEL%
)

echo Linking the object file...
gcc -m32 output.obj -o program.exe -lmsvcrt -lkernel32
if %ERRORLEVEL% NEQ 0 (
    echo Error linking
    pause
    exit /b %ERRORLEVEL%
)

echo Running the final program...
program.exe
if %ERRORLEVEL% NEQ 0 (
    echo Error running program
    pause
    exit /b %ERRORLEVEL%
)

echo Build completed successfully!
pause 