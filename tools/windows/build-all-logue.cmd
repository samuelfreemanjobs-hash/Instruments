@echo off
setlocal
cd /d "%~dp0..\.."
where bash >nul 2>&1
if errorlevel 1 (
  echo bash not found. Install Git for Windows, then re-run.
  exit /b 1
)
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build-all-logue.ps1" %*
exit /b %ERRORLEVEL%
