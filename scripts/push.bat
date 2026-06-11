@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0.."

echo ==========================================
echo   PixelHero - Git Push to GitHub
echo ==========================================
echo.

:: ---- Commit Message ----
set "MSG=%~1"
if "%MSG%"=="" (
    echo [提示] 未提供提交信息，使用默认信息
    for /f %%i in ('git log --oneline -1 2^>nul ^|^| echo "initial"') do set "LAST=%%i"
    set /p MSG="请输入提交信息: "
    if "!MSG!"=="" set "MSG=update"
)

echo.
echo [1/3] Staging changes...
git add -A
if errorlevel 1 (
    echo git add FAILED!
    pause
    exit /b 1
)

echo [2/3] Committing...
git commit -m "!MSG!"
if errorlevel 1 (
    echo git commit FAILED or nothing to commit.
)

echo [3/3] Pushing to origin/main...
git push origin HEAD:main
if errorlevel 1 (
    echo [警告] git push FAILED! 请检查网络或远程仓库配置。
    pause
    exit /b 1
)

echo.
echo ==========================================
echo   上传完成!
echo ==========================================
echo.
exit /b 0
