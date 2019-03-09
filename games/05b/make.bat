copy ..\..\engine\3rd\SDL2.dll /y 
cl %* -DENGINE_C ..\..\engine\engine.c -I ..\..\engine ..\..\engine\3rd\SDL2.lib