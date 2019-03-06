#define GLFAST_IMPLEMENTATION
#include "glfast.h"

#define STR(x) #x
#define countof(x) (sizeof(x) / sizeof((x)[0]))

i32 main()
{
  SDL_Window  * sdl_window;
  SDL_GLContext sdl_glcontext;
  
  gfWindow(&sdl_window, &sdl_glcontext, 0, 0, "App", 1280, 720, 4);
  
  gpu_storage_t mesh = {0};
  mesh.format = xyz_f32;
  mesh.count  = 3;
  mesh = gfStorageCreateFromStruct(mesh);
  
  mesh.as_vec3[0].x = -0.5f;
  mesh.as_vec3[0].y = -0.5f;
  mesh.as_vec3[0].z =  0.0f;
  
  mesh.as_vec3[1].x =  0.0f;
  mesh.as_vec3[1].y =  0.5f;
  mesh.as_vec3[1].z =  0.0f;
  
  mesh.as_vec3[2].x =  0.5f;
  mesh.as_vec3[2].y = -0.5f;
  mesh.as_vec3[2].z =  0.0f;
  
  gpu_cmd_t cmd[1] = {0};
  
  cmd[0].count = mesh.count;
  cmd[0].instance_count = 1;
  
  const char * vs_str = VERT_HEAD STR
  (
    layout(binding = 0) uniform samplerBuffer in_pos;
    
    void main()
    {
      vec3 pos = texelFetch(in_pos, gl_VertexID).xyz;
      gl_Position = vec4(pos, 1.f);
    }
  );
  
  const char * fs_str = FRAG_HEAD STR
  (
    out vec4 color;
    
    void main()
    {
      color = vec4(1.f);
    }
  );

  u32 vs = gfProgramCreateFromString(GL_VERTEX_SHADER, vs_str);
  u32 fs = gfProgramCreateFromString(GL_FRAGMENT_SHADER, fs_str);
  u32 pp = gfProgramPipelineCreate(vs, fs);
  
  u32 state_textures[16] = { mesh.id };
  
  glBindTextures(0, 16, state_textures);
  
  while(1)
  {
    gfClear();
    gfDraw(pp, countof(cmd), cmd);
    
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT)
        goto exit;
    }
    
    SDL_GL_SwapWindow(sdl_window);
    glFinish();
  }
  
  exit: return 0;
}
