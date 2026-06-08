@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========== 像素勇者 - 构建 ==========
echo.

set "MINGW=E:\MinGW-w64\mingw64\bin"
set "QT=E:\QT\6.11.1\mingw_64\bin"
set "PATH=%MINGW%;%QT%;%PATH%"

cd /d "%~dp0"

echo [1/4] 生成 Makefile...
qmake.exe pixel_hero.pro
if errorlevel 1 (
    echo qmake 失败！
    goto :error
)
echo 已完成

echo [2/4] 修复 Makefile (MinGW 16.1.0兼容)...
powershell -Command "(Get-Content Makefile.Release -Raw) -replace '-DQT_NEEDS_QMAIN', '' -replace 'E:/QT/6.11.1/mingw_64/lib/libQt6EntryPoint.a', '' | Set-Content Makefile.Release -NoNewline"
if errorlevel 1 (
    echo Makefile 修复失败！
    goto :error
)
echo 已完成

echo [3/4] 编译...
mingw32-make.exe -f Makefile.Release -j4
if errorlevel 1 goto :error

echo [4/4] 构建成功！
echo.
if exist "bin\pixel_hero.exe" (
    echo 可执行文件: %cd%\bin\pixel_hero.exe
) else if exist "release\pixel_hero.exe" (
    echo 可执行文件: %cd%\release\pixel_hero.exe
)
goto :end

:error
echo.
echo ========== 构建失败 ==========
pause
exit /b 1

:end
echo.
pause
