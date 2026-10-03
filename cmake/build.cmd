@echo off
if exist build rmdir /s /q build
cmake -S "%~dp0." -B "%~dp0build" -A x64
cmake --build "%~dp0build" --config Release
pause