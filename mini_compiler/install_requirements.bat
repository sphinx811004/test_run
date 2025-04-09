@echo off
echo Installing requirements for mini compiler...

REM Create a temp directory for downloads
if not exist "temp" mkdir temp
cd temp

echo Downloading NASM...
powershell -Command "Invoke-WebRequest -Uri 'https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/win64/nasm-2.16.01-win64.zip' -OutFile 'nasm.zip'"

echo Extracting NASM...
powershell -Command "Expand-Archive -Path 'nasm.zip' -DestinationPath 'nasm_temp'"

echo Installing NASM...
xcopy /E /I /Y nasm_temp\nasm-2.16.01 C:\nasm
setx PATH "%PATH%;C:\nasm"

echo Downloading MinGW-w64...
powershell -Command "Invoke-WebRequest -Uri 'https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev1.7z' -OutFile 'mingw.7z'"

echo Extracting MinGW-w64...
powershell -Command "Expand-Archive -Path 'mingw.7z' -DestinationPath 'mingw_temp'"

echo Installing MinGW-w64...
xcopy /E /I /Y mingw_temp\mingw64 C:\mingw64
setx PATH "%PATH%;C:\mingw64\bin"

echo Cleaning up...
cd ..
rmdir /S /Q temp

echo.
echo Installation complete!
echo Please restart your terminal for PATH changes to take effect.
echo.
echo Verifying installations...
echo.

echo Testing NASM...
nasm --version
if %ERRORLEVEL% NEQ 0 (
    echo NASM installation failed!
) else (
    echo NASM is properly installed
)

echo.
echo Testing GCC...
gcc --version
if %ERRORLEVEL% NEQ 0 (
    echo GCC installation failed!
) else (
    echo GCC is properly installed
)

echo.
echo If you see any errors above, please run setup_path.bat manually.
pause 