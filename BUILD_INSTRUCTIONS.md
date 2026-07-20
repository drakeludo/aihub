# Build Instructions

## Prerequisites

1. **Visual Studio 2022** with C++ Desktop Development
2. **vcpkg** installed at `C:\vcpkg`
3. **CMake 3.25+**

## Quick Build

### Option 1: Using CMake GUI (Recommended)

1. Open CMake GUI
2. Set source path: `C:\Users\-\Music\AIHub`
3. Set build path: `C:\Users\-\Music\AIHub\build`
4. Click "Configure"
5. Choose generator: **Visual Studio 17 2022**
6. Set toolchain file: `C:/vcpkg/scripts/buildsystems/vcpkg.cmake`
7. Click "Generate"
8. Click "Open Project"
9. In Visual Studio: Build > Build Solution

### Option 2: Command Line

```powershell
# From Developer Command Prompt for VS 2022
cd C:\Users\-\Music\AIHub
mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build . --config Release
```

### Option 3: NMake (if Visual Studio generator doesn't work)

```powershell
# From Developer Command Prompt for VS 2022
cd C:\Users\-\Music\AIHub
mkdir build
cd build

cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

nmake
```

## Dependencies (Auto-installed by vcpkg)

- imgui
- spdlog
- sqlite3

## Output

Executable will be at:
- `build/Release/AIHub.exe` (Visual Studio)
- `build/AIHub.exe` (NMake)

## Troubleshooting

If CMake can't find Visual Studio:
1. Make sure you're running from **Developer Command Prompt for VS 2022**
2. Or use CMake GUI instead
3. Or try NMake generator

If vcpkg fails:
1. Update vcpkg: `cd C:\vcpkg && git pull && .\bootstrap-vcpkg.bat`
2. Try again

## Features Implemented

✅ Modern C++23 architecture
✅ Event-driven system
✅ Service layer (Chat, Theme, History, Export)
✅ 5 cyberpunk themes
✅ Particle system
✅ Performance monitoring
✅ Animation system
✅ Export/Import (Markdown, JSON, HTML, TXT)
✅ Hotkey support
✅ Thread pool
✅ SQLite ready

🚧 Browser automation (WebView2) - next phase
🚧 AI provider integration - next phase

## Next Steps After Build

ChatGPT will help you finish:
1. WebView2 integration
2. Browser automation for AI providers
3. Full compilation and testing
