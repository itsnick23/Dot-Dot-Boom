@echo off
setlocal enabledelayedexpansion

:: Detectar MinGW / G++
set "MINGW_BIN="
if exist "C:\Program Files\CodeBlocks\MinGW\bin\g++.exe" (
    set "MINGW_BIN=C:\Program Files\CodeBlocks\MinGW\bin"
) else if exist "%USERPROFILE%\tools\mingw64\bin\g++.exe" (
    set "MINGW_BIN=%USERPROFILE%\tools\mingw64\bin"
) else if exist "C:\tools\mingw64\bin\g++.exe" (
    set "MINGW_BIN=C:\tools\mingw64\bin"
) else if exist "C:\Users\USER\tools\mingw64\bin\g++.exe" (
    set "MINGW_BIN=C:\Users\USER\tools\mingw64\bin"
)

if not "%MINGW_BIN%"=="" (
    set "PATH=%MINGW_BIN%;%PATH%"
)

if not exist src mkdir src

echo Compilando prueba de backend en consola...
g++ -std=c++20 Celda.cpp Chapa.cpp Inventario.cpp Tablero.cpp Partida.cpp test\prueba.cpp -o src\prueba.exe
if errorlevel 1 (
    echo [ERROR] Fallo la compilacion de pruebas.
    exit /b 1
)

echo Ejecutando prueba...
src\prueba.exe
