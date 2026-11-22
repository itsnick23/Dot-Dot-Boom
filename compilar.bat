@echo off
setlocal enabledelayedexpansion

:: 1. Detectar MinGW / G++
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

:: 2. Detectar SFML 3
set "SFML="
if exist "%USERPROFILE%\tools\SFML-3.1.0" (
    set "SFML=%USERPROFILE%\tools\SFML-3.1.0"
) else if exist "C:\tools\SFML-3.1.0" (
    set "SFML=C:\tools\SFML-3.1.0"
) else if exist "C:\Users\USER\tools\SFML-3.1.0" (
    set "SFML=C:\Users\USER\tools\SFML-3.1.0"
)

if "%SFML%"=="" (
    echo [ERROR] No se encontro SFML 3.1.0 en las rutas estandar.
    exit /b 1
)

if not exist src mkdir src

:: Copiar DLLs necesarias
if exist "%SFML%\bin" (
    copy /Y "%SFML%\bin\sfml-*-3.dll" src >nul 2>nul
)
if not "%MINGW_BIN%"=="" (
    copy /Y "%MINGW_BIN%\libgcc_s_seh-1.dll" src >nul 2>nul
    copy /Y "%MINGW_BIN%\libstdc++-6.dll" src >nul 2>nul
    copy /Y "%MINGW_BIN%\libwinpthread-1.dll" src >nul 2>nul
)

echo Compilando Dot-Dot-Boom con SFML...
g++ -std=c++20 Celda.cpp Chapa.cpp Inventario.cpp Tablero.cpp Partida.cpp main.cpp -o src\DotDotBoom.exe -I"%SFML%\include" -L"%SFML%\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
if errorlevel 1 (
    echo [ERROR] Fallo la compilacion.
    exit /b 1
)

echo Compilacion exitosa: src\DotDotBoom.exe

if not "%1"=="nojugar" start "" /D src src\DotDotBoom.exe
