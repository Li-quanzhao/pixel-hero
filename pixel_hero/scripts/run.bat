@echo off
setlocal

set "MINGW=E:\MinGW-w64\mingw64\bin"
set "QT=E:\QT\6.11.1\mingw_64\bin"
set "PATH=%QT%;%MINGW%;%PATH%"

cd /d "%~dp0.."

if not exist "bin\pixel_hero.exe" (
    echo pixel_hero.exe not found! Run build.bat first.
    pause
    exit /b 1
)

echo Starting Pixel Hero...
echo.
start "" "bin\pixel_hero.exe"
