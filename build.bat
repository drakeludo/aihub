@echo off
echo ========================================
echo   AIHub - Build Script
echo ========================================

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

cd /d "%~dp0"

if not exist build mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

if errorlevel 1 (
    echo [ERROR] CMake failed
    pause
    exit /b 1
)

cmake --build . --config Release

if errorlevel 1 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo   SUCCESS: build\Release\AIHub.exe
echo ========================================
pause
