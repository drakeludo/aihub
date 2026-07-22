"""Configuration management for AIHub."""
import json
from pathlib import Path
from typing import Dict, Any

DEFAULT_SETTINGS = {
    "data_dir": str(Path.home() / ".aihub"),
    "theme": "dark",
    "language": "ru",
    "auto_save": True,
    "log_level": "INFO",
    "window_size": [1200, 800],
    "window_pos": [100, 100],
}

def get_config_path() -> Path:
    data_dir = Path(DEFAULT_SETTINGS["data_dir"])
    data_dir.mkdir(parents=True, exist_ok=True)
    return data_dir / "config.json"


def load_settings() -> Dict[str, Any]:
    config_path = get_config_path()
    if config_path.exists():
        try:
            with open(config_path, 'r', encoding='utf-8') as f:
                settings = json.load(f)
            return {**DEFAULT_SETTINGS, **settings}
        except Exception:
            pass
    return DEFAULT_SETTINGS.copy()

def save_settings(settings: Dict[str, Any]):
    config_path = get_config_path()
    with open(config_path, 'w', encoding='utf-8') as f:
        json.dump(settings, f, ensure_ascii=False, indent=2)