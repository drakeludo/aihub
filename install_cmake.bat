@echo off
echo Downloading CMake...

powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/Kitware/CMake/releases/download/v3.28.1/cmake-3.28.1-windows-x86_64.zip' -OutFile 'cmake.zip'}"

echo Extracting CMake...
powershell -Command "Expand-Archive -Path cmake.zip -DestinationPath C:\cmake -Force"

del cmake.zip

echo CMake installed to C:\cmake\cmake-3.28.1-windows-x86_64\bin
echo.
echo Adding to PATH for this session...
set PATH=C:\cmake\cmake-3.28.1-windows-x86_64\bin;%PATH%

echo Done!
pause
