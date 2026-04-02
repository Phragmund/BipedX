@echo off

REM 编译仿真环境
gcc simulation.c -o simulation.exe -I"../Product/SDL2-devel-2.32.0-mingw/SDL2-2.32.0/i686-w64-mingw32/include" -L"../Product/SDL2-devel-2.32.0-mingw/SDL2-2.32.0/i686-w64-mingw32/lib" -lSDL2main -lSDL2 -lopengl32 -lglu32

REM 检查编译是否成功
if %ERRORLEVEL% == 0 (
    echo Compilation successful!
    echo Running simulation environment...
    simulation.exe
) else (
    echo Compilation failed. Please check code and dependencies.
)

pause