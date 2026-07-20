# AIHub

**Современный AI-агрегатор для Windows с ImGui интерфейсом**

## 🚀 Особенности

- **100% бесплатно** - MIT лицензия, без Qt
- **ImGui + DirectX 11** - нативный Windows UI
- **vcpkg** - автоматическая установка зависимостей
- **C++23** - современный стандарт
- **Visual Studio 2022** - поддержка последней версии

## 📋 Требования

- Windows 10/11
- Visual Studio 2022 с C++ инструментами
- Git
- CMake 3.25+

## 🛠️ Установка и сборка

### 1. Клонировать репозиторий

```bash
git clone <ваш-репо-url>
cd AIHub
```

### 2. Установить vcpkg (если еще нет)

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

### 3. Собрать проект

```powershell
# Запустить автоматическую сборку
.\compile_now.bat
```

Скрипт автоматически:
- Настроит Visual Studio окружение
- Установит зависимости через vcpkg (imgui, spdlog)
- Соберет проект
- Создаст `build\Release\AIHub.exe`

## 📦 Зависимости

Все устанавливаются автоматически через vcpkg:

- **imgui** - UI библиотека
- **spdlog** - логирование

## 🏗️ Структура проекта

```
AIHub/
├── src/
│   ├── main_imgui.cpp      # Точка входа
│   ├── Application.cpp/h   # Основное приложение
│   ├── UI/                 # ImGui окна
│   ├── Logger/             # Логирование
│   └── ...
├── external/
│   └── imgui/              # ImGui submodule
├── CMakeLists.txt          # CMake конфигурация
├── vcpkg.json              # Зависимости vcpkg
└── compile_now.bat         # Скрипт сборки
```

## 🎯 Roadmap

- [x] ImGui интерфейс
- [x] DirectX 11 рендеринг
- [x] vcpkg интеграция
- [ ] WebView2 для браузеров AI
- [ ] Поддержка ChatGPT, DeepSeek, Kiro
- [ ] База данных чатов
- [ ] Экспорт в Markdown/JSON

## 📝 Лицензия

MIT License - используй как хочешь!

## 🤝 Контрибьют

Pull requests приветствуются! Для больших изменений сначала открой issue.

---

**Собрано с ❤️ в 2026 году**
