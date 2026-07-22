# AIHub

Desktop application for DeepSeek web interface with project memory.

## Installation

1. Install dependencies:
   ```bash
   pip install -r requirements.txt
   playwright install
   ```

2. Run:
   ```bash
   python main.py
   ```

## Features
- Browser automation with Playwright
- Project-based memory with SQLite
- Smart context injection
- Modern PySide6 UI

## Project Structure
- `core/`: Core logic
- `browser/`: Browser management
- `driver/`: DeepSeek interaction
- `memory/`: Project memory
- `ui/`: User interface
- etc.