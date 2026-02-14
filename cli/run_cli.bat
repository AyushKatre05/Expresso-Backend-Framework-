@echo off
REM Expresso Backend Framework - Project Manager Launcher
REM Quick launcher for the interactive Expresso project CLI

title Expresso Backend Framework - Manager
echo.
echo Launching Expresso Project Manager...
echo.

python "%~dp0project_cli.py"

if errorlevel 1 (
    echo.
    echo Error: Failed to run Python script
    echo.
    echo Make sure you have:
    echo  - Python 3.7+ installed (https://www.python.org/)
    echo  - requests library: pip install requests
    echo  - CMake 3.13+ installed
    echo.
    pause
)
