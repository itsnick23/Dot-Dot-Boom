@echo off
set TOOLS=C:\Users\USER\tools
set PATH=%TOOLS%\mingw64\bin;%PATH%
set SFML=%TOOLS%\SFML-3.1.0

if not exist src mkdir src
copy /Y "%SFML%\bin\*.dll" src >nul
copy /Y "%TOOLS%\mingw64\bin\libgcc_s_seh-1.dll" src >nul
copy /Y "%TOOLS%\mingw64\bin\libstdc++-6.dll" src >nul
copy /Y "%TOOLS%\mingw64\bin\libwinpthread-1.dll" src >nul

g++ -std=c++20 *.cpp -o src\DotDotBoom.exe -I"%SFML%\include" -L"%SFML%\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
if errorlevel 1 exit /b 1

if not "%1"=="nojugar" start "" /D src src\DotDotBoom.exe
