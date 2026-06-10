@echo off
setlocal enabledelayedexpansion

set "MINGW=E:\MinGW-w64\mingw64\bin"
set "QT=E:\QT\6.11.1\mingw_64\bin"
set "QTDLLS=E:\QT\6.11.1\mingw_64\bin"
set "MINGWDLLS=E:\MinGW-w64\mingw64\bin"
set "PATH=%MINGW%;%QT%;%PATH%"

cd /d "%~dp0.."

echo ==========================================
echo   PixelHero - Build ^& Deploy ^& Run
echo ==========================================
echo.

:: ---- Clean ----
echo [1/5] Cleaning...
if exist Makefile.Release del /q Makefile.Release 2>nul
if exist Makefile.Debug   del /q Makefile.Debug 2>nul
if exist Makefile         del /q Makefile 2>nul
if exist build            rmdir /s /q build 2>nul
if exist release          rmdir /s /q release 2>nul

:: ---- qmake ----
echo [2/5] Running qmake...
"%QT%\qmake.exe" pixel_hero.pro 2>nul
if not exist Makefile.Release (
    echo qmake FAILED! Check: "%QT%\qmake.exe"
    pause
    exit /b 1
)

:: ---- Patch ----
echo [3/5] Patching Makefile...
set "E1=E:\QT\6.11.1\mingw_64\lib\libQt6EntryPoint.a"
set "E2=E:/QT/6.11.1/mingw_64/lib/libQt6EntryPoint.a"
powershell -NoProfile -Command ^
    "$f='Makefile.Release';$t=(Get-Content $f -Raw);$t=$t.Replace('-DQT_NEEDS_QMAIN','').Replace('%E1%','').Replace('%E2%','');Set-Content -Path $f -Value $t -NoNewline;Write-Host '  Patched'"

:: ---- Compile ----
echo [4/5] Compiling...
mingw32-make.exe -f Makefile.Release -j4
if %errorlevel% neq 0 (
    echo.
    echo Build FAILED! errorlevel=%errorlevel%
    pause
    exit /b 1
)

echo Build SUCCESS!

:: ---- Deploy DLLs (only if missing) ----
echo [5/5] Deploying DLLs...
if exist bin\Qt6Core.dll (
    echo   DLLs already deployed, skipping windeployqt.
) else (
    echo   Running windeployqt...
    "%QT%\windeployqt.exe" --no-compiler-runtime --no-translations bin\pixel_hero.exe 2>nul
    if %errorlevel% neq 0 (
        echo   windeployqt failed, copying DLLs manually...
        copy /y "%QTDLLS%\Qt6Core.dll"        bin\        >nul 2>nul
        copy /y "%QTDLLS%\Qt6Gui.dll"         bin\        >nul 2>nul
        copy /y "%QTDLLS%\Qt6Widgets.dll"     bin\        >nul 2>nul
        copy /y "%QTDLLS%\Qt6Network.dll"     bin\        >nul 2>nul
        copy /y "%QTDLLS%\Qt6Svg.dll"         bin\        >nul 2>nul
        copy /y "%QTDLLS%\opengl32sw.dll"     bin\        >nul 2>nul
        copy /y "%QTDLLS%\D3Dcompiler_47.dll" bin\        >nul 2>nul
        copy /y "%MINGWDLLS%\libgcc_s_seh-1.dll"  bin\  >nul 2>nul
        copy /y "%MINGWDLLS%\libstdc++-6.dll"      bin\  >nul 2>nul
        copy /y "%MINGWDLLS%\libwinpthread-1.dll"  bin\  >nul 2>nul
    )
)

:: ---- qt.conf ----
if not exist bin\qt.conf (
    if exist qt.conf (
        copy /y qt.conf bin\qt.conf >nul 2>nul
    )
)

:: ---- Run ----
echo Starting pixel_hero.exe...
powershell -NoProfile -Command "& '.\bin\pixel_hero.exe'"
