#include <stdio.h>

#define GLFAST_IMPLEMENTATION
#include "glfast.h"
#define BOB_IMPLEMENTATION
#include "bob.h"
#define QFPC_IMPLEMENTATION
#include "qfpc.h"

#define STR(x) #x
#define countof(x) (sizeof(x) / sizeof((x)[0]))

#define MAX_STR 5000

typedef struct
{
  union { f32 x; f32 u; };
  union { f32 y; f32 v; };
} vec2;

typedef struct
{
  union { f32 x; f32 r; };
  union { f32 y; f32 g; };
  union { f32 z; f32 b; };
} vec3;

typedef struct
{
  union { f32 x; f32 r; };
  union { f32 y; f32 g; };
  union { f32 z; f32 b; };
  union { f32 w; f32 a; };
} vec4;

typedef struct
{
  f32 sd_x, up_x, fw_x;
  f32 sd_y, up_y, fw_y;
  f32 sd_z, up_z, fw_z;
} mat3;

union
{
  char c[1];
  struct
  {
    char teapot_bob [MAX_STR];
    char sphere_bob [MAX_STR];
    char monkey_bob [MAX_STR];
    char vs_mesh    [MAX_STR];
    char fs_mesh    [MAX_STR];
    char vs_quad    [MAX_STR];
    char fs_quad    [MAX_STR];
    char vs_cubemap [MAX_STR];
    char fs_cubemap [MAX_STR];
    char texture_1  [MAX_STR];
    char texture_2  [MAX_STR];
    char texture_3  [MAX_STR];
    char cubemap_px [MAX_STR];
    char cubemap_nx [MAX_STR];
    char cubemap_py [MAX_STR];
    char cubemap_ny [MAX_STR];
    char cubemap_pz [MAX_STR];
    char cubemap_nz [MAX_STR];
  };
} RESRC =
{
  .monkey_bob = "data/meshes/monkey.bob",
  .sphere_bob = "data/meshes/sphere.bob",
  .teapot_bob = "data/meshes/teapot.bob",
  .vs_mesh    = "data/shaders/mesh.vert",
  .fs_mesh    = "data/shaders/mesh.frag",
  .vs_quad    = "data/shaders/quad.vert",
  .fs_quad    = "data/shaders/quad.frag",
  .vs_cubemap = "data/shaders/cubemap.vert",
  .fs_cubemap = "data/shaders/cubemap.frag",
  .texture_1  = "data/textures/texture_1.bmp",
  .texture_2  = "data/textures/texture_2.bmp",
  .texture_3  = "data/textures/texture_3.bmp",
  .cubemap_px = "data/textures/cubemap_right.bmp",
  .cubemap_nx = "data/textures/cubemap_left.bmp",
  .cubemap_py = "data/textures/cubemap_top.bmp",
  .cubemap_ny = "data/textures/cubemap_bottom.bmp",
  .cubemap_pz = "data/textures/cubemap_front.bmp",
  .cubemap_nz = "data/textures/cubemap_back.bmp"
};

void Perspective(
  f32 * proj,
  f32 aspect,
  f32 fov_y_rad,
  f32 n,
  f32 f)
{
  f32 d = 1.f / (f32)tan(fov_y_rad * 0.5f);
  proj[0] = d / aspect;
  proj[1] = d;
  proj[2] = n / (n - f);
  proj[3] = (f * n) / (n - f);
}

f32 Aspect(SDL_Window * sdl_window)
{
  i32 w, h;
  SDL_GetWindowSize(sdl_window, &w, &h);
  return w / (f32)h;
}

i32 main(i32 ArgCount, char ** Args)
{
  char * path_exe = SDL_GetBasePath();
  for(u32 i = 0, size = sizeof(RESRC); i < size; i += MAX_STR)
  {
    char path_res[MAX_STR];
    SDL_memcpy(path_res, &RESRC.c[i], MAX_STR);
    SDL_snprintf(&RESRC.c[i], MAX_STR, "%s%s", path_exe, path_res);
  }
  
  SDL_Window  * sdl_window;
  SDL_GLContext sdl_glcontext;
  
  gfWindow(&sdl_window, &sdl_glcontext, 0, 0, "App", 1280, 720, 4);

  const char * bobs[] =
  {
    RESRC.monkey_bob,
    RESRC.sphere_bob,
    RESRC.teapot_bob,
  };

  bob_t meshes = gfBobCreate(countof(bobs), bobs);

  const char * bmps[] =
  {
    RESRC.texture_1,
    RESRC.texture_2,
    RESRC.texture_3,
  };

  gpu_texture_t textures = gfTextureCreateFromBmp(512, 512, 4, countof(bmps), bmps);
  
  const char * cubemap_px[] = { RESRC.cubemap_px };
  const char * cubemap_nx[] = { RESRC.cubemap_nx };
  const char * cubemap_py[] = { RESRC.cubemap_py };
  const char * cubemap_ny[] = { RESRC.cubemap_ny };
  const char * cubemap_pz[] = { RESRC.cubemap_pz };
  const char * cubemap_nz[] = { RESRC.cubemap_nz };
  
  gpu_texture_t cubemaps = gfCubemapCreateFromBmp(512, 512, 4, countof(cubemap_px),
    cubemap_px, cubemap_nx, cubemap_py, cubemap_ny, cubemap_pz, cubemap_nz
  );
  
  u32 vs_mesh = gfProgramCreateFromFile(GL_VERTEX_SHADER, RESRC.vs_mesh);
  u32 fs_mesh = gfProgramCreateFromFile(GL_FRAGMENT_SHADER, RESRC.fs_mesh);
  u32 pp_mesh = gfProgramPipelineCreate(vs_mesh, fs_mesh);
  
  u32 vs_quad = gfProgramCreateFromFile(GL_VERTEX_SHADER, RESRC.vs_quad);
  u32 fs_quad = gfProgramCreateFromFile(GL_FRAGMENT_SHADER, RESRC.fs_quad);
  u32 pp_quad = gfProgramPipelineCreate(vs_quad, fs_quad);
  
  u32 vs_cubemap = gfProgramCreateFromFile(GL_VERTEX_SHADER, RESRC.vs_cubemap);
  u32 fs_cubemap = gfProgramCreateFromFile(GL_FRAGMENT_SHADER, RESRC.fs_cubemap);
  u32 pp_cubemap = gfProgramPipelineCreate(vs_cubemap, fs_cubemap);
  
  gpu_cmd_t cmd[3] = {0};
  
  cmd[0].first = meshes.first.as_u32[0];
  cmd[1].first = meshes.first.as_u32[1];
  cmd[2].first = meshes.first.as_u32[2];
  
  cmd[0].count = meshes.count.as_u32[0];
  cmd[1].count = meshes.count.as_u32[1];
  cmd[2].count = meshes.count.as_u32[2];
  
  cmd[0].instance_first = 0;
  cmd[1].instance_first = 30;
  cmd[2].instance_first = 60;
  
  cmd[0].instance_count = 30;
  cmd[1].instance_count = 30;
  cmd[2].instance_count = 30;
  
  gpu_storage_t ins_first = gfStorageCreate(.format = x_u32, .count = countof(cmd));
  gpu_storage_t ins_pos = gfStorageCreate(.format = xyz_f32, .count = 90);
  
  for(u32 i = 0; i < ins_first.count; ++i)
  {
    ins_first.as_u32[i] = cmd[i].instance_first;
  }
  
  for(u32 i = 0, row = 10, space = 3; i < 90; ++i)
  {
    ins_pos.as_vec3[i].x = i * space - (i / row) * row * space;
    ins_pos.as_vec3[i].y = 0;
    ins_pos.as_vec3[i].z = (i / row) * space;
  }

  gpu_texture_t fbo_depth = gfTextureCreate(.w = 1280, 720, .format = depth_b32);
  gpu_texture_t fbo_color = gfTextureCreate(.w = 1280, 720, .format = srgba_b8);
  
  u32 fbo_colors[] =
  {
    [0] = fbo_color.id,
  };
  
  u32 fbo = gfFboCreate(fbo_depth.id, 0, countof(fbo_colors), fbo_colors, 0);
  
  gpu_sampler_t s_textures = gfSamplerCreate(4);
  gpu_sampler_t s_fbo = gfSamplerCreate(.min = GL_NEAREST, GL_NEAREST);
  
  u32 state_textures[16] =
  {
    [0] = meshes.mesh_id.id,
    [1] = meshes.attr_first.id,
    [2] = meshes.attr_id.id,
    [3] = meshes.pos.id,
    [4] = meshes.uv.id,
    [5] = meshes.normal.id,
    [6] = ins_first.id,
    [7] = ins_pos.id,
    [8] = textures.id,
    [9] = cubemaps.id,
   [10] = fbo_color.id,
  };
  
  u32 state_samplers[16] =
  {
    [8] = s_textures.id,
    [9] = s_textures.id,
   [10] = s_fbo.id,
  };
  
  glBindTextures(0, 16, state_textures);
  glBindSamplers(0, 16, state_samplers);
  
  vec3 cam_pos = {23.518875f, 5.673130f, 26.649000f};
  vec4 cam_rot = {-0.351835f, 0.231701f, 0.090335f, 0.902411f};
  vec4 cam_prj = {0.f};
  mat3 cam_mat = {0.f};

  Perspective(
    &cam_prj.x,
    Aspect(sdl_window),
    85.f * QFPC_TO_RAD,
    0.01f, 1000.f
  );
  
  SDL_SetRelativeMouseMode(1);
  u32 t_prev = SDL_GetTicks();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);

  while(1)
  {
    u32 t_curr = SDL_GetTicks();
    f64 dt = ((t_curr - t_prev) * 60.0) / 1000.0;
    
    SDL_PumpEvents();
    i32 mouse_x_rel = 0;
    i32 mouse_y_rel = 0;
    SDL_GetRelativeMouseState(&mouse_x_rel, &mouse_y_rel);
    const u8 * key = SDL_GetKeyboardState(NULL);
    
    quatFirstPersonCamera(
      &cam_pos.x,
      &cam_rot.x,
      &cam_mat.sd_x,
      0.10f,
      0.05f * (f32)dt,
      mouse_x_rel,
      mouse_y_rel,
      key[SDL_SCANCODE_W],
      key[SDL_SCANCODE_A],
      key[SDL_SCANCODE_S],
      key[SDL_SCANCODE_D],
      key[SDL_SCANCODE_E],
      key[SDL_SCANCODE_Q]
    );
    
    static int show_pass = 0;
    if(key[SDL_SCANCODE_1]) show_pass = 0;
    if(key[SDL_SCANCODE_2]) show_pass = 1;
    if(key[SDL_SCANCODE_3]) show_pass = 2;
    if(key[SDL_SCANCODE_4]) show_pass = 3;
    if(key[SDL_SCANCODE_5]) show_pass = 4;
    
    glProgramUniform3fv(vs_mesh, 0, 1, &cam_pos.x);
    glProgramUniform4fv(vs_mesh, 1, 1, &cam_rot.x);
    glProgramUniform4fv(vs_mesh, 2, 1, &cam_prj.x);
    glProgramUniform3fv(fs_mesh, 0, 1, &cam_pos.x);
    glProgramUniform1iv(fs_mesh, 1, 1, &show_pass);
    glProgramUniform4fv(vs_cubemap, 0, 1, &cam_rot.x);
    glProgramUniform4fv(vs_cubemap, 1, 1, &cam_prj.x);
    
    for(u32 i = 0; i < 90; ++i)
      ins_pos.as_vec3[i].y = (f32)sin((t_curr * 0.0015f) + (i * 0.5f)) * 0.3f;
    
    gfFboBind(fbo);
      gfClear();
      gfDraw(pp_mesh, countof(cmd), cmd);
    gfFboBind(0);
    
    gfClear();
    
    if(!show_pass)
    {
      glDisable(GL_DEPTH_TEST);
        gfFire(pp_cubemap, 36);
      glEnable(GL_DEPTH_TEST);
    }
    
    gfFire(pp_quad, 6);

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT)
        goto exit;
    }
    
    SDL_GL_SwapWindow(sdl_window);
    glFinish();
    
    t_prev = t_curr;
  }

  exit: return 0;
}
