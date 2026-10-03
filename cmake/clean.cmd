@echo off
set "ROOT=%~dp0.."
if exist "%~dp0build" rmdir /s /q "%~dp0build"
if exist "%ROOT%\bin"  rmdir /s /q "%ROOT%\bin"