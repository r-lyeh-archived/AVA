copy ..\..\engine\3rd\SDL2.dll /y 
cl demo_instancing.c -I ..\..\engine\3rd ..\..\engine\3rd\SDL2.lib %* 
