@echo off
echo ========================================
echo   AIHub - Build Script
echo ========================================

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

cd /d "%~dp0"

if not exist build mkdir build
cd build

echo [INFO] Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

if errorlevel 1 (
    echo [ERROR] CMake configuration failed
    echo [INFO] Trying NMake instead...
    cd ..
    rmdir /s /q build
    mkdir build
    cd build
    cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
    if errorlevel 1 (
        echo [ERROR] CMake failed with both generators
        pause
        exit /b 1
    )
    nmake
    if errorlevel 1 (
        echo [ERROR] Build failed
        pause
        exit /b 1
    )
    goto SUCCESS
)

echo [INFO] Building with Visual Studio...
cmake --build . --config Release

if errorlevel 1 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

:SUCCESS
echo.
echo ========================================
echo   Build Successful!
echo ========================================

if exist "Release\AIHub.exe" (
    echo Executable: build\Release\AIHub.exe
    echo.
    echo [INFO] Copying DLL dependencies...
    if exist "vcpkg_installed\x64-windows\bin" (
        xcopy /Y /Q "vcpkg_installed\x64-windows\bin\*.dll" "Release\"
    )
    if exist "C:\vcpkg\installed\x64-windows\bin" (
        xcopy /Y /Q "C:\vcpkg\installed\x64-windows\bin\*.dll" "Release\"
    )
) else if exist "AIHub.exe" (
    echo Executable: build\AIHub.exe
    echo.
    echo [INFO] Copying DLL dependencies...
    if exist "vcpkg_installed\x64-windows\bin" (
        xcopy /Y /Q "vcpkg_installed\x64-windows\bin\*.dll" ".\"
        echo DLL copied from vcpkg_installed
    )
    if exist "C:\vcpkg\installed\x64-windows\bin" (
        xcopy /Y /Q "C:\vcpkg\installed\x64-windows\bin\*.dll" ".\"
        echo DLL copied from C:\vcpkg
    )
)

echo.
echo [INFO] Creating logs directory...
if exist "Release\" (
    mkdir "Release\logs" 2>nul
) else (
    mkdir "logs" 2>nul
)

echo.
echo ========================================
echo   Ready to run!
echo ========================================
pause
