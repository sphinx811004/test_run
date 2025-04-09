@echo off
echo Setting up PATH for NASM and MinGW...

REM Check if NASM is installed
where nasm >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo NASM not found! Please enter the path where you extracted NASM:
    echo Example: C:\nasm
    set /p nasm_path=NASM Path: 
    setx PATH "%PATH%;%nasm_path%"
)

REM Check if GCC is installed
where gcc >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo GCC not found! Please enter the path where you extracted MinGW:
    echo Example: C:\mingw64\bin
    set /p mingw_path=MinGW Path: 
    setx PATH "%PATH%;%mingw_path%"
)

echo.
echo Checking installations...
echo.

echo Testing NASM...
nasm --version
if %ERRORLEVEL% NEQ 0 (
    echo NASM is not properly installed or not in PATH
) else (
    echo NASM is properly installed
)

echo.
echo Testing GCC...
gcc --version
if %ERRORLEVEL% NEQ 0 (
    echo GCC is not properly installed or not in PATH
) else (
    echo GCC is properly installed
)

echo.
echo Note: You may need to restart your terminal for PATH changes to take effect.
pause 