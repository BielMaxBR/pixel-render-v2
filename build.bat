@echo off
if not exist build cmake -G "MinGW Makefiles" -DGRAPHICS_API_OPENGL_11 -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build

echo Compilando...
cmake --build build && build\out\PixelRender.exe
