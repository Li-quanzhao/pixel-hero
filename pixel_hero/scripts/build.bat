@echo off
setlocal enabledelayedexpansion

echo ========================================== 
echo   PixelHero - One-Click Build 
echo ========================================== 
echo.

set "MINGW=E:\MinGW-w64\mingw64\bin"
set "QT=E:\QT\6.11.1\mingw_64\bin"
set "PATH=%MINGW%;%QT%;%PATH%"

cd /d "%~dp0.."

:: Step 1: Clean
echo [1/4] Cleaning old build...
if exist Makefile.Release  del /q Makefile.Release
if exist Makefile.Debug    del /q Makefile.Debug
if exist Makefile          del /q Makefile
if exist build             (rmdir /s /q build  2>nul)
if exist release           (rmdir /s /q release 2>nul)
echo Done.
echo.

:: Step 2: qmake
echo [2/4] Running qmake...
"%QT%\qmake.exe" pixel_hero.pro
if not exist Makefile.Release (
    echo qmake FAILED - Makefile.Release not generated!
    echo Check: "%QT%\qmake.exe"
    pause
    exit /b 1
)
echo Done.
echo.

:: Step 3: Patch Makefile
echo [3/4] Patching Makefile...
set "ENTRY_BACK=E:\QT\6.11.1\mingw_64\lib\libQt6EntryPoint.a"
set "ENTRY_FWD=E:/QT/6.11.1/mingw_64/lib/libQt6EntryPoint.a"
powershell -NoProfile -Command ^
    "$f='Makefile.Release'; $t=(Get-Content $f -Raw); $t=$t.Replace('-DQT_NEEDS_QMAIN','').Replace('%ENTRY_BACK%','').Replace('%ENTRY_FWD%',''); Set-Content -Path $f -Value $t -NoNewline; Write-Host '  Patched OK'"
echo Done.
echo.

:: Step 4: Compile
echo [4/4] Compiling...
mingw32-make.exe -f Makefile.Release -j4
if %errorlevel% neq 0 (
    echo.
    echo ==========================================
    echo   Build FAILED! errorlevel=%errorlevel%
    echo ==========================================
    pause
    exit /b 1
)

echo.
echo ==========================================
echo  Build SUCCESS!
echo ==========================================
echo.

:: Copy qt.conf to bin/ (Qt needs it next to exe)
if exist qt.conf (
    copy /y qt.conf bin\qt.conf >nul
    echo Copied qt.conf to bin\ 
)

echo Output: bin\pixel_hero.exe
echo Run with: scripts\run.bat
echo.
pause
