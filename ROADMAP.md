# AIHub Development Roadmap

## 🎯 Vision
Центр управления ИИ в стиле Cyberpunk + Gaming + AI Lab.
Не "ещё один чат", а полноценный космический корабль для работы с AI.

---

## Phase 1: Core Architecture (Сейчас здесь)

### 1.1 Service Layer
- [ ] BrowserService - управление браузерами
- [ ] ChatService - работа с сообщениями
- [ ] HistoryService - история чатов
- [ ] SettingsService - настройки
- [ ] ThemeService - темы
- [ ] ExportService - экспорт
- [ ] LoggerService - логирование

### 1.2 Data Models
- [ ] ChatMessage - сообщение
- [ ] Provider - провайдер AI
- [ ] Conversation - диалог
- [ ] Attachment - вложение
- [ ] Prompt - промпт
- [ ] UserSettings - настройки пользователя

### 1.3 Event System
- [ ] EventBus - центральная шина событий
- [ ] События: MessageSent, ResponseReceived, BrowserReady, Error, LoadComplete, AuthLost

### 1.4 BrowserManager Enhancement
- [ ] Автовосстановление после падения
- [ ] Проверка открытой страницы
- [ ] Ожидание загрузки
- [ ] Проверка авторизации
- [ ] Очередь запросов
- [ ] Таймауты
- [ ] Отмена зависших запросов
- [ ] Журнал действий

---

## Phase 2: Core Features

### 2.1 Message System
- [ ] Потоковая генерация
- [ ] Отмена генерации
- [ ] Повторная генерация
- [ ] Редактирование
- [ ] Копирование
- [ ] Закрепление
- [ ] Поиск

### 2.2 History (SQLite)
- [ ] Схема БД
- [ ] История чатов
- [ ] Настройки
- [ ] Закрепленные чаты
- [ ] Последние модели
- [ ] Параметры приложения

### 2.3 Performance
- [ ] Thread Pool
- [ ] Кэширование
- [ ] Ленивая загрузка истории
- [ ] Виртуальный список сообщений
- [ ] Асинхронная загрузка файлов
- [ ] Фоновые задачи

---

## Phase 3: Advanced Features

### 3.1 Export/Import
- [ ] Export: Markdown, TXT, HTML, JSON, PDF
- [ ] Import: Markdown, TXT, JSON

### 3.2 Code Handling
- [ ] Подсветка синтаксиса
- [ ] Кнопка копирования
- [ ] Определение языка
- [ ] Сворачивание блоков
- [ ] Номера строк

### 3.3 File Handling
- [ ] Drag & Drop
- [ ] Просмотр изображений
- [ ] Просмотр PDF
- [ ] Просмотр Markdown
- [ ] Просмотр текста

### 3.4 Hotkeys
- [ ] Новая сессия
- [ ] Поиск
- [ ] Отправка
- [ ] Остановка генерации
- [ ] Переключение чатов
- [ ] Переключение темы
- [ ] Настройки

---

## Phase 4: Visual Revolution 🎨

### 4.1 Theme System
- [ ] Cyber
- [ ] Neon
- [ ] Matrix
- [ ] Synthwave
- [ ] Tech Lab

### 4.2 UI Components Library
- [ ] Buttons
- [ ] Cards
- [ ] Panels
- [ ] Dialogs
- [ ] Switches
- [ ] Inputs

### 4.3 Animation System
- [ ] Window open/close
- [ ] Message appear
- [ ] Typing effect
- [ ] Hover effects
- [ ] Loading animations
- [ ] Tab switching
- [ ] Menu transitions
- [ ] Resize animations

### 4.4 Effects
- [ ] Bloom
- [ ] Glow
- [ ] Blur
- [ ] Neon
- [ ] Particles
- [ ] Ripple
- [ ] Hover Glow
- [ ] Shadow Animation

### 4.5 Background
- [ ] Animated particles
- [ ] Grid
- [ ] Light spots
- [ ] Noise
- [ ] Gradient animation

### 4.6 Panels Redesign
- [ ] Top Panel: Logo, FPS, Memory, Browser Status, Queue, CPU
- [ ] Left Panel: Chat cards with glow effects
- [ ] Right Panel: Logs, Browsers, Queue, Memory, Stats (closeable)
- [ ] Bottom Panel: Large input with attachments, settings, indicators

### 4.7 Chat Redesign
- [ ] Card-style messages
- [ ] Color-coded by AI
- [ ] Model icons
- [ ] Timestamps
- [ ] Status indicators
- [ ] Action buttons
- [ ] Glow effects

### 4.8 Monitoring Dashboard
- [ ] Live graphs: CPU, RAM, Browser, Queue
- [ ] Response speed
- [ ] Generation time
- [ ] Real-time updates

### 4.9 Startup Screen
- [ ] Beautiful animation
- [ ] Module initialization
- [ ] Progress indicators

### 4.10 Notification Center
- [ ] Animated messages
- [ ] Event notifications
- [ ] Status updates

---

## Phase 5: Polish

### 5.1 Error System
- [ ] Beautiful error dialogs
- [ ] Helpful messages
- [ ] Recovery suggestions

### 5.2 Settings
- [ ] Theme
- [ ] Language
- [ ] UI Scale
- [ ] Hotkeys
- [ ] Font size
- [ ] Auto-save
- [ ] Export folder
- [ ] Log level

### 5.3 Advanced Logging
- [ ] Categories: Core, Browser, UI, Database, Network, JavaScript, Performance, Errors
- [ ] Filtering
- [ ] Search

---

## Design Philosophy

**NOT:**
- Корпоративный минимализм
- Скучный белый интерфейс
- "Ещё один чат"

**YES:**
- Центр управления космическим кораблём
- Живые графики и индикаторы
- Свечение, анимации, частицы
- Ощущение работы сложной системы
- Cyberpunk + AI Lab aesthetic

---

## Tech Stack

- **Language:** C++23
- **UI:** ImGui + DirectX 11
- **Database:** SQLite
- **Browser:** WebView2
- **Package Manager:** vcpkg
- **Build:** CMake + Visual Studio 2022

---

## Current Status

✅ Basic ImGui application
✅ DirectX 11 rendering
✅ Logger system
✅ Basic UI windows
✅ vcpkg integration

🚧 Service layer
🚧 Data models
🚧 Event system
🚧 Browser management
🚧 History system
🚧 Visual design

---

**Собрано с 🔥 в 2026 году**
