@rem or also, /Dinline=__forceinline /fp:fast /Ox /Ot , also /Ox /Oy

set OPTIMS=
if "%1"=="dev" (set OPTIMS=/DDEBUG /Zi /Oy- /MDd) else (set OPTIMS=/DNDEBUG /O2 /Oy /MT)

cl  *.c* ^
    3rd\@ocornut\*.cpp ^
    3rd\@ocornut\examples\imgui_impl_glfw.cpp ^
    3rd\@ocornut\examples\imgui_impl_opengl3.cpp ^
    3rd\@ocornut\examples\libs\gl3w\GL\gl3w.c ^
    -I . ^
    -I imgui\ ^
    -I 3rd\@ocornut\ ^
    -I 3rd\@ocornut\examples ^
    -I 3rd\@ocornut\examples\libs\gl3w ^
    -I 3rd\@ocornut\examples\example_glfw_opengl3 ^
    -I ..\engine\app\3rd ^
    -I ..\engine ..\engine\engine.c -DENGINE_C ^
    %OPTIMS% /EHsc %*

del *.obj
del *.ilk
