# AIHub

**Современный AI-агрегатор для Windows с ImGui интерфейсом**

## 🚀 Особенности

- **100% бесплатно** - MIT лицензия, без Qt
- **ImGui + DirectX 11** - нативный Windows UI
- **vcpkg** - автоматическая установка зависимостей
- **C++23** - современный стандарт
- **Visual Studio 2022** - поддержка последней версии

## 📋 Требования

- Windows 10/11 (x64)
- Visual Studio 2022 с "Desktop development with C++"
- Git
- CMake 3.25+
- vcpkg (будет установлен автоматически если нужно)

## 🛠️ Установка и сборка

### 1. Клонировать репозиторий

```bash
git clone https://github.com/drakeludo/aihub.git
cd aihub
```

### 2. Установить vcpkg (если еще нет)

```powershell
# Клонировать vcpkg в C:\vcpkg
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
```

### 3. Собрать проект

```powershell
# Перейти в папку проекта
cd C:\путь\к\aihub

# Запустить автоматическую сборку
.\build.bat
```

Скрипт автоматически:
- Настроит Visual Studio 2022 окружение
- Установит зависимости через vcpkg (imgui, spdlog, sqlite3, fmt)
- Соберет проект с CMake (NMake Makefiles)
- Скопирует DLL файлы
- Создаст `build\AIHub.exe`

### 4. Запустить

```powershell
cd build
.\AIHub.exe
```

## ⚙️ Альтернативная сборка (вручную)

```powershell
# Создать папку build
mkdir build
cd build

# Настроить CMake с vcpkg
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Собрать
nmake

# Скопировать DLL
xcopy /Y vcpkg_installed\x64-windows\bin\*.dll .\
```

## 📦 Зависимости

Все устанавливаются автоматически через vcpkg:

- **imgui 1.92.8** - UI библиотека
- **spdlog 1.17.0** - логирование  
- **sqlite3 3.53.3** - база данных (для будущего функционала)
- **fmt 12.2.0** - форматирование строк

## 🔧 Требования к системе

- **OS**: Windows 10/11 (x64)
- **Compiler**: Visual Studio 2022 Community/Professional/Enterprise
- **CMake**: 3.25 или выше
- **vcpkg**: Установлен в `C:\vcpkg`
- **Git**: Для клонирования репозитория
- **DirectX 11**: Встроен в Windows 10/11

## ❗ Решение проблем

### Проблема: "CMake не найден"
**Решение**: Убедитесь что CMake установлен и добавлен в PATH, или используйте Visual Studio Developer Command Prompt.

### Проблема: "vcpkg не найден"
**Решение**: Установите vcpkg в `C:\vcpkg` по инструкции выше, либо измените путь в `build.bat`:
```powershell
-DCMAKE_TOOLCHAIN_FILE=C:/ваш/путь/к/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Проблема: "Программа не запускается / missing DLL"
**Решение**: Убедитесь что DLL скопированы:
```powershell
cd build
xcopy /Y vcpkg_installed\x64-windows\bin\*.dll .\
```

### Проблема: "Компиляция без варнингов"
**Решение**: Проект настроен на **ZERO WARNINGS**. Все неиспользуемые параметры подавлены через `(void)param`.

### Логи программы
Логи сохраняются в:
```
C:\Users\ВАШ_ЮЗЕР\AppData\Local\Temp\AIHub\logs\aihub.log
```

## 🏗️ Структура проекта

```
AIHub/
├── src/
│   ├── main_imgui.cpp      # Точка входа
│   ├── Application.cpp/h   # Основное приложение (Window + D3D11 + ImGui)
│   ├── Core/               # PerformanceMonitor, EventBus, ThreadPool
│   ├── Models/             # ChatMessage, Conversation, Provider
│   ├── Services/           # ChatService, ThemeService, HistoryService, ExportService
│   ├── UI/                 # ChatWindow, SidebarWindow, LogWindow, StatusBar
│   └── Logger/             # Логирование в temp
├── external/
│   └── imgui/              # ImGui submodule (Git)
├── build/                  # Собранные файлы (не в Git)
│   ├── AIHub.exe           # Исполняемый файл
│   ├── *.dll               # DLL зависимостей
│   └── logs/               # Логи выполнения
├── CMakeLists.txt          # CMake конфигурация
├── vcpkg.json              # Манифест зависимостей vcpkg
├── build.bat               # Скрипт автоматической сборки
└── README.md               # Эта инструкция
```

## 🎯 Roadmap

- [x] ImGui интерфейс + DirectX 11
- [x] vcpkg интеграция с автоматической установкой зависимостей
- [x] Архитектура: Models, Services, Events, UI components
- [x] 5 кибер-тем (Cyber, Neon, Matrix, Synthwave, TechLab)
- [x] Performance monitoring + Status bar
- [x] Logger с записью в temp папку
- [x] ThreadPool для фоновых задач
- [x] Export/Import (Markdown, JSON, HTML, TXT)
- [ ] SQLite интеграция для истории чатов
- [ ] WebView2 для браузерной автоматизации AI
- [ ] Поддержка ChatGPT, DeepSeek, Claude, Kiro
- [ ] Hotkey manager для быстрых действий
- [ ] Streaming ответов от AI
- [ ] Поиск по истории чатов

## 📝 Лицензия

MIT License - используй как хочешь!

## 🤝 Контрибьют

Pull requests приветствуются! Для больших изменений сначала открой issue.

---

**Собрано с ❤️ в 2026 году**
