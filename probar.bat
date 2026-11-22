@echo off
set TOOLS=C:\Users\USER\tools
set PATH=%TOOLS%\mingw64\bin;%PATH%

if not exist src mkdir src
g++ -std=c++20 Celda.cpp Chapa.cpp Inventario.cpp Tablero.cpp Partida.cpp test\prueba.cpp -o src\prueba.exe
if errorlevel 1 exit /b 1

src\prueba.exe
