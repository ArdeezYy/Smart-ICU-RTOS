@echo off
REM ========================================
REM Smart ICU RTOS - Dashboard + ngrok Starter
REM ========================================
echo [Smart ICU] Starting Flask dashboard...
cd /d "%~dp0"
start "Flask Dashboard" cmd /c "python app.py"

echo [Smart ICU] Waiting for Flask to start...
timeout /t 3 /nobreak >nul

echo [Smart ICU] Starting ngrok tunnel...
ngrok http 5000 --region=ap

echo.
echo == Smart ICU is running! ==
echo Flask:  http://localhost:5000
echo ngrok:  Check http://localhost:4040 for tunnel URL
