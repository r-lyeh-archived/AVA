#include "ava.h"

#ifdef AVA_C
#pragma once
#define CORE_C
#define FRAMEWORK_C
#define AUDIO_C
#define ASSET_C
#define MEDIA_C
#define RENDER_C
#define SCENE_C
#define WORLD_C
#endif

#include "core/core.c"

#include "framework/framework.c"

#include "media/media.c"

#include "framework/audio/audio.c"
#include "framework/asset/asset.c"

#include "render/render.c"
#include "scene/scene.c"
#include "world/world.c"



API int init();

int init() {
    import("game/**");
    import("assets/**");
    void (*ptr)();
    for( int i = 0; i < 128; ++i ) {
        if( 0 != (ptr = dllquick( "game.dll", "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( va("game%d.dll", i), "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( va("game%02d.dll", i), "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( va("game%03d.dll", i), "main" ) ) ) {
            ptr();
        }
    }
    return 0;
}
