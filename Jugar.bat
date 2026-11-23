@echo off
setlocal
cd /d "%~dp0"

echo ===================================================
echo             DOT-DOT-BOOM (Lanzador)
echo ===================================================

if exist "src\DotDotBoom.exe" (
    echo Iniciando el juego...
    start "" /D . src\DotDotBoom.exe
    exit /b 0
) else (
    echo [AVISO] No se encontro src\DotDotBoom.exe ya compilado.
    echo Intentando compilar el proyecto...
    call compilar.bat
)
