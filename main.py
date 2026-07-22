import sys
from PySide6.QtWidgets import QApplication
from aihub.ui.main_window import MainWindow
from aihub.config.settings import load_settings
from aihub.logger.setup import setup_logging

def main():
    setup_logging()
    settings = load_settings()
    
    app = QApplication(sys.argv)
    window = MainWindow(settings)
    window.show()
    
    sys.exit(app.exec())

if __name__ == "__main__":
    main()