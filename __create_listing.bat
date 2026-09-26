@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

set "OUTPUT_FILE=__programm.txt"
set "CURRENT_DIR=%CD%"

if exist "%OUTPUT_FILE%" del "%OUTPUT_FILE%"

echo Building listing from .cpp, .hpp, .md files recursively...
echo.

set COUNT=0

for /r %%F in (*.cpp *.hpp *.md) do (
    set /a COUNT+=1
    set "FULL_PATH=%%F"

    set "REL_PATH=!FULL_PATH:%CURRENT_DIR%\=!"

    if "!REL_PATH!"=="!FULL_PATH!" set "REL_PATH=!FULL_PATH:%CURRENT_DIR%=!"

    if "!REL_PATH!"=="!FULL_PATH!" (
        for %%A in ("%%F") do set "REL_PATH=%%~pnxA"
    )

    if "!REL_PATH:~0,1!"=="\" set "REL_PATH=!REL_PATH:~1!"

    echo [!COUNT!] Processing: !REL_PATH!

    echo =============================================================================== >> "%OUTPUT_FILE%"
    echo File: !REL_PATH! >> "%OUTPUT_FILE%"
    echo =============================================================================== >> "%OUTPUT_FILE%"
    echo. >> "%OUTPUT_FILE%"

    type "%%F" >> "%OUTPUT_FILE%"

    echo. >> "%OUTPUT_FILE%"
    echo. >> "%OUTPUT_FILE%"
)

echo.
echo Done! Processed files: !COUNT!
echo Result saved to %OUTPUT_FILE%

pause