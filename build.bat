@echo off
if not exist build cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build

echo Compilando...
cmake --build build

echo Rodando o jogo...
build\out\game.exe
pause
