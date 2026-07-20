@echo off
echo ========================================
echo   AI Hub - Build Script
echo ========================================
echo.

REM Настрой путь к Qt (ИЗМЕНИ НА СВОЙ!)
set QT_PATH=C:\Qt\6.5.3\msvc2019_64

if not exist "%QT_PATH%" (
    echo [ERROR] Qt not found at: %QT_PATH%
    echo Please edit build.bat and set correct QT_PATH
    pause
    exit /b 1
)

echo [INFO] Qt found: %QT_PATH%
echo.

REM Создай папку build
if not exist build (
    echo [INFO] Creating build directory...
    mkdir build
)

cd build

echo [INFO] Running CMake...
cmake .. -DCMAKE_PREFIX_PATH=%QT_PATH% -G "Ninja"

if errorlevel 1 (
    echo [ERROR] CMake failed
    cd ..
    pause
    exit /b 1
)

echo.
echo [INFO] Building project...
cmake --build .

if errorlevel 1 (
    echo [ERROR] Build failed
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo   Build successful!
echo ========================================
echo.
echo Executable: build\AIHub.exe
echo.
echo Run: build\AIHub.exe
echo.

cd ..
pause
