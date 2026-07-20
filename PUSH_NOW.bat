@echo off
echo ========================================
echo   Push to GitHub
echo ========================================
echo.
echo Repository: https://github.com/drakeludo/aihub
echo.
echo If you get authentication error, do this:
echo.
echo 1. Install GitHub CLI:
echo    winget install GitHub.cli
echo.
echo 2. Login:
echo    gh auth login
echo.
echo 3. Push:
echo    gh repo sync
echo.
echo OR use GitHub Desktop:
echo    winget install GitHub.GitHubDesktop
echo.
echo ========================================
echo.
echo Trying to push now...
cd /d "%~dp0"
git push -u origin main
pause
