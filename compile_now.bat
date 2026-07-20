@echo off
echo ========================================
echo   AIHub - Modern Build 2026
echo   Using vcpkg + Visual Studio 2022
echo ========================================
echo.

REM Setup Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

cd /d "c:\Users\-\Music\AIHub"

REM Add CMake to PATH
set PATH=c:\Users\-\Music\AIHub\cmake-portable\cmake-3.31.3-windows-x86_64\bin;%PATH%

REM Create build directory
if not exist build mkdir build
cd build

echo.
echo [INFO] Configuring with CMake + vcpkg...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

if errorlevel 1 (
    echo [ERROR] CMake configuration failed
    pause
    exit /b 1
)

echo.
echo [INFO] Building project...
cmake --build . --config Release

if errorlevel 1 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo   Build Successful!
echo ========================================
echo.
echo Executable: build\Release\AIHub.exe
echo.
pause
