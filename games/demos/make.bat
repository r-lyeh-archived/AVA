@echo off
for /f "tokens=1,* delims= " %%a in ("%*") do set OPTIONS=%%b
copy ..\..\engine\3rd\SDL2.dll /y > nul
if "%1"==""      ( echo make [compiler_flags] [demo_name^|all^|run^|clean]) else ^
if "%1"=="clean" ( del *.exp *.lib *.obj *.dll *.exe *.pdb *.ilk sdl2.dll ) else ^
if "%1"=="all"   ( for %%i in (*.c) do call make %OPTIONS% %%i) else ^
if "%1"=="run"   ( for %%i in (*.exe) do start /wait %%i ) else ^
cl /nologo /EHsc /MP -DENGINE_C -I. -I..\..\engine %* ..\..\engine\engine.c ..\..\engine\3rd\SDL2.lib
set OPTIONS=
