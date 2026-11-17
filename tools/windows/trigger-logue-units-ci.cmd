@echo off
setlocal
cd /d "%~dp0..\.."
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0..\..\scripts\antigravity-bridge\run-logue-units-ci.ps1" %*
exit /b %ERRORLEVEL%
