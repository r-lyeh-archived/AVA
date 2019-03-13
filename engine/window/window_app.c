#ifndef APP_H
#define APP_H

#include "../render/render.c" // OPENGL
#define SDL_MAIN_HANDLED
#include "../3rd/SDL2/SDL.h" // SDL2
// #include "../3rd/glfast.h" // SDL2+OPENGL(subset)

extern SDL_Window *window;

enum {
    WINDOW_LEGACY_OPENGL = 0x4,
    WINDOW_SQUARED = 0x8,
    WINDOW_NO_MOUSE = 0x10,
    WINDOW_MSAA4 = 0x20,
};

API int window_create( float zoom /* 10.0f */, int flags );
API void window_title( const char *title );
API int window_update();
API int* window_size();
API void window_swap( void **pixels );
API void window_destroy();

enum {
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
};

API double *window_get( int variable );


#endif


#ifdef APP_C
#pragma once
#include "../input/input.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

int (*printf_handler)(const char *fmt, ...) = printf;

static void die_callback( const char *text ) {
    fprintf(stderr, "%s\n", text);
    exit(-1);
}
static void error_callback(int error, const char* description) {
    int whitelisted = !!strstr(description, "Failed to create OpenGL context");
    if(whitelisted) return;
    fprintf(stderr, "app error %#x: %s\n", error, description);
}
/*
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
static void* glfwGetProcAddressExtraCompat(const char *name) {
    void *ptr = glfwGetProcAddress(name);
    if( !ptr ) {
        char buf[128];
        sprintf(buf, "%sARB", name);
        ptr = glfwGetProcAddress(buf);
        if( ptr ) printf("%s -> %s", name, buf );
    }
    if( !ptr ) {
        char buf[128];
        sprintf(buf, "%sEXT", name);
        ptr = glfwGetProcAddress(buf);
        if( ptr ) printf("%s -> %s", name, buf );
    }
    if( !ptr ) {
        printf("%s not found\n", name);
    }
    return ptr;
}
*/

/*
#define GLFAST_EXTERNAL_LOADER
#define GLFAST_IMPLEMENTATION
#include "../3rd/glfast.h" // OPENGL(subset)
*/

#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242

#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E

#if 0
static void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    /*
    // whitelisted codes
    if( id == 131169 ) return;
    if( id == 131185 ) return;
    if( id == 131204 ) return;
    if( id == 131218 ) return;
    */

    if( severity == GL_DEBUG_SEVERITY_HIGH ) {
        printf_handler("!glDebugCallback: %s (%d) (source=%s, type=%s, severity=%s) #OPENGL\n", message, id,
            source == GL_DEBUG_SOURCE_API               ? "API" :
            source == GL_DEBUG_SOURCE_APPLICATION       ? "Application" :
            source == GL_DEBUG_SOURCE_OTHER             ? "Other" :
            source == GL_DEBUG_SOURCE_SHADER_COMPILER   ? "Shader Compiler" :
            source == GL_DEBUG_SOURCE_THIRD_PARTY       ? "Third Party" :
            source == GL_DEBUG_SOURCE_WINDOW_SYSTEM     ? "Window System" : "",

            type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR   ? "Deprecated Behavior" :
            type == GL_DEBUG_TYPE_ERROR                 ? "Error" :
            type == GL_DEBUG_TYPE_MARKER                ? "Marker" :
            type == GL_DEBUG_TYPE_OTHER                 ? "Other" :
            type == GL_DEBUG_TYPE_PERFORMANCE           ? "Performance" :
            type == GL_DEBUG_TYPE_POP_GROUP             ? "Pop Group" :
            type == GL_DEBUG_TYPE_PORTABILITY           ? "Portability" :
            type == GL_DEBUG_TYPE_PUSH_GROUP            ? "Push Group" :
            type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR    ? "Undefined Behavior" : "",

            severity == GL_DEBUG_SEVERITY_HIGH          ? "High" :
            severity == GL_DEBUG_SEVERITY_LOW           ? "Low" :
            severity == GL_DEBUG_SEVERITY_MEDIUM        ? "Medium" :
            severity == GL_DEBUG_SEVERITY_NOTIFICATION  ? "Notification" : "");
//      exit(-1);
    }
}

void trap_gl() {
#ifndef SHIPPING
    if (!glDebugMessageCallback) return;
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if( flags & GL_CONTEXT_FLAG_DEBUG_BIT ) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC)glDebugCallback, /*NULL*/0);
        //GL( glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, /*NULL*/0, GL_TRUE) );
        printf_handler("%s #OPENGL\n", "Debug output initialized.");
    } else {
        printf_handler("%s #OPENGL\n", "Debug output not supported.");
    }
#endif
}

#else

void glDebugCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t length, const char * message, void * userdata) {
    const char * GL_ERROR_SOURCE[] = { "API", "WINDOW SYSTEM", "SHADER COMPILER", "THIRD PARTY", "APPLICATION", "OTHER" };
    const char * GL_ERROR_SEVERITY[] = { "HIGH", "MEDIUM", "LOW", "NOTIFICATION" };
    const char * GL_ERROR_TYPE[] = { "ERROR", "DEPRECATED BEHAVIOR", "UNDEFINED DEHAVIOUR", "PORTABILITY", "PERFORMANCE", "OTHER" };

    SDL_Log( "%s [ID: %u]\n", message, id );
    /* "[SEVERITY: %s] [SOURCE: %s] [TYPE: %s]", GL_ERROR_SEVERITY[
          severity != GL_DEBUG_SEVERITY_NOTIFICATION ?
          severity  - GL_DEBUG_SEVERITY_HIGH : 3
        ],
        GL_ERROR_SOURCE[source - GL_DEBUG_SOURCE_API],
        GL_ERROR_TYPE[type - GL_DEBUG_TYPE_ERROR], */
}

void trap_gl() {

}

#endif


/*static*/ SDL_Window  * window = NULL;
/*static*/ SDL_GLContext glcontext;
/*static*/ char title[128] = {0};
static int should_quit = 0;
static int window_flags = 0;

void window_title( const char *title_ ) {
    if( title_ ) strcpy(title, title_);
}

void window_destroy(void) {
    render_quit();
    if(window) {
        SDL_DestroyWindow(window);
        window = 0;
        should_quit = 0;
    }
    // if( num_active_windows == 0 )
    // glfwTerminate(); // exit(0)
}
int window_create( float zoom, int flags ) {
    if( window ) {
        return 0;
    } else {
        // engine init
        init();

/* #if !EDITOR
#ifdef _MSC_VER
        _chdir( SDL_GetBasePath() );
#else
        chdir( SDL_GetBasePath() );
#endif
*/

        // atexit(window_destroy);
    }

#ifdef _MSC_VER
    int arg0len = strlen( __argv[0] );
    if( arg0len > 4 ) {
        char *dot = &__argv[0][arg0len - 4];
        if( 0 == strcmp( dot, ".exe" ) || 0 == strcmp( dot, ".EXE" ) ) {
            *dot = '\0';
        }
    }
#endif

    flags |= WINDOW_LEGACY_OPENGL; // O:)

    window_flags = flags;

    // detect monitors
    SDL_Rect bounds;
    int monitor = 0;
    for( ; monitor < SDL_GetNumVideoDisplays(); ++monitor ) {
        SDL_GetDisplayBounds( monitor, &bounds );
        if(!bounds.x && !bounds.y) break; // primary monitor found
    }

    // window screen coverage (zoom)
    // zoom = 1..100%, or [0.f .. 1.f] ; fullscreen if zoom = 100% or 1.f ; borderless-fullscreen if zoom > 100% (and also if 1.1f or negative zoom)
    // @todo: remove [0..1] range?
    int fullscreen = 0;
    zoom = zoom > 1.1 ? zoom / 100.f : zoom;
    zoom = zoom > 0 && zoom < 1 ? zoom : (fullscreen = 1, 1);

    int appw = (int)(bounds.w * zoom), apph = (int)(bounds.h * zoom);
    if( flags & WINDOW_SQUARED ) {
        if( appw > apph ) appw = apph;
        else apph = appw;
    }
    int w = appw, h = apph;

    /* flags
    SDL_WINDOW_FULLSCREEN - fullscreen window
    SDL_WINDOW_FULLSCREEN_DESKTOP - fullscreen window at the current desktop resolution
    SDL_WINDOW_OPENGL - window usable with OpenGL context
    SDL_WINDOW_VULKAN - window usable with a Vulkan instance
    SDL_WINDOW_HIDDEN - window is not visible
    SDL_WINDOW_BORDERLESS - no window decoration
    SDL_WINDOW_RESIZABLE - window can be resized
    SDL_WINDOW_MINIMIZED - window is minimized
    SDL_WINDOW_MAXIMIZED - window is maximized
    SDL_WINDOW_INPUT_GRABBED - window has grabbed input focus
    SDL_WINDOW_ALLOW_HIGHDPI - window should be created in high-DPI mode if supported (>= SDL 2.0.1)
    */
    int sdl_window_flags = SDL_WINDOW_OPENGL;
    sdl_window_flags |= SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;
    sdl_window_flags |= fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    sdl_window_flags |= fullscreen ? 0 : SDL_WINDOW_RESIZABLE;
    sdl_window_flags |= fullscreen ? SDL_WINDOW_INPUT_GRABBED : 0; // grabbed -> mouse confined to window
    // glfwWindowHint(GLFW_REFRESH_RATE, desktop->refreshRate);

    if( flags & WINDOW_MSAA4 ) {
        int msaa_samples = 4;
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa_samples);
    }

    /*
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    */

    /*
    glfwWindowHint(GLFW_RED_BITS, desktop->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, desktop->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, desktop->blueBits);
    glfwWindowHint(GLFW_STENCIL_BITS, 32); // 0 == GLFW_DONT_CARE
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    */

    // #ifndef RELEASE
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    // #endif

    int have_debug = GL_FALSE;
    int have_core = flags & WINDOW_LEGACY_OPENGL ? SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : SDL_GL_CONTEXT_PROFILE_CORE;

    // try core first, unless compat stated
    if( !(flags & WINDOW_LEGACY_OPENGL) ) {
        int majors[] = { 4, 4, 4, 4, 4, 3, 3 };
        int minors[] = { 4, 3, 2, 1, 0, 3, 2 };
        for( int i = 0; !window && i < sizeof(majors) / sizeof(majors[0]); ++i ) {
            int majv = majors[i], minv = minors[i];
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majv);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minv);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, have_core ); // SDL_GL_CONTEXT_PROFILE_CORE);

        #if 1 // !SHIPPING
            // only in +4.3
            // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG); have_debug = GL_TRUE;
        #endif

            window = SDL_CreateWindow(
                "",
                bounds.x + (bounds.w - appw) / 2, bounds.y + (bounds.h - apph) / 2,
                appw, apph, sdl_window_flags
            );

            if(window) printf("opengl %d.%d%s%s context created\n", majv, minv, have_core ? "-core" : "", have_debug ? "-debug" : "");
        }
    }

    // else 2.1 compat
    if( !window ) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE ); // GLFW_OPENGL_COMPAT_PROFILE );
        /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG); */ have_debug = GL_FALSE;

        window = SDL_CreateWindow(
            "",
            bounds.x + (bounds.w - appw) / 2, bounds.y + (bounds.h - apph) / 2,
            appw, apph, sdl_window_flags
        );

        if(window) printf("opengl 2.1%s%s context created\n", 0 ? "-core" : "", have_debug ? "-debug" : "");
    }

    if (!window) {
        die_callback("Error: Cannot create window (SDL_CreateWindow)");
    }

    glcontext = SDL_GL_CreateContext(window);
    if( !glcontext ) {
        die_callback("Error: Cannot create GL context (SDL_GL_CreateContext)"); //, SDL_GetError());
    }

    SDL_GL_MakeCurrent(window, glcontext);

#  if defined __gl3w_h_
    gl3w_init();
#elif defined __glad_h_
    // gladLoadGLLoader((GLADloadproc) glfwGetProcAddressExtraCompat);

    // Initialize OpenGL loader
    // bool err = false;
    // bool err = gl3wInit() != 0;
    // bool err = glewInit() != GLEW_OK;
    bool err = gladLoadGL() == 0;
    if (err) {
        die_callback("Error: Cannot initialize OpenGL loader");
    }
#else
    puts("warning: no opengl loader found");
#endif

    // intel gma hd
    // if(!glGenVertexArrays) glGenVertexArrays = (void*)glfwGetProcAddressExtraCompat("glGenVertexArrays");
    // if(!glBindVertexArray) glBindVertexArray = (void*)glfwGetProcAddressExtraCompat("glBindVertexArray");

    SDL_GL_SetSwapInterval(1); // Enable vsync, also check -1

    //glfwSetKeyCallback(window, key_callback);

#if 0
    // center
    if( desktop ) {
        glfwSetWindowPos(window , (desktop->width-appw)/2 , (desktop->height-apph)/2);
    }
#endif

    // Enable the debug callback
    // #ifndef RELEASE
    typedef void (*GLDEBUGPROC)(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t length, const char * message, const void * userParam);
    typedef void (*GLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void * userParam);
    void (*glDebugMessageCallback)(GLDEBUGPROC callback, const void * userParam) = (GLDEBUGMESSAGECALLBACKPROC)SDL_GL_GetProcAddress("glDebugMessageCallback");
    glDebugMessageCallback((GLDEBUGPROC)glDebugCallback, NULL);
    // #endif

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    render_init();

    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);
    //glGetIntegerv(GL_MAX_SAMPLES, &max_supported_samples);

    extern void set_mouse(int);
    set_mouse( flags & WINDOW_NO_MOUSE ? 'hide' : 'show' );

    return 1;
}
int* window_size() {
    static int dim[2];
    SDL_GL_GetDrawableSize(window, &dim[0], &dim[1]);
    return dim;
}
int window_update() {
    if( should_quit ) {
        return 0;
    }
    int *rect = window_size();
    int width = rect[0], height = rect[1];
    float ratio = width / (height + 1.f);
    render_update(width, height);
    mouse_update();

    return 1;
}

void render_capture( int w, int h, int comp, void *pixels );

void window_swap( void **pixels ) {
    int *rect = window_size();
    int w = rect[0], h = rect[1];

    render_post(w, h);

    SDL_GL_SwapWindow(window);
    glFinish();

    // SDL_PumpEvents();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            should_quit = 1;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            should_quit = 1;
        if (event.type == SDL_MOUSEMOTION && (window_flags & WINDOW_NO_MOUSE) ) {
            /*
            extern double mx,my;
            mx += event.motion.xrel;
            my += event.motion.yrel;
            int *rects = window_size();
            SDL_WarpMouseInWindow(window, rects[0]/2, rects[1]/2);
            */
        }
    }

    if( pixels && !should_quit ) {
        *pixels = (unsigned char *)realloc(*pixels, 4 * w * h);
        render_capture(w,h,3,*pixels);
    }

    static double frames = 0, begin = FLT_MAX, fps = 0, prev_frame = 0;
    double now = SDL_GetTicks() / 1000.0; // glfwGetTime();
    if( begin > now ) {
        begin = now;
        frames = 0;
    }
    if( (now - begin) >= 0.25f ) {
        fps = frames * (1.f / (now - begin));
    }
    if( (now - begin) > 1 ) {
        begin = now + ((now - begin) - 1);
        frames = 0;
    }
    if( title[0] ) {
        SDL_SetWindowTitle(window, title);
    } else {
        char buf[128] = {0};
        #ifdef _MSC_VER
        sprintf(buf, "%s %2dfps %5.2fms", __argv[0], (int)fps, (now - prev_frame) * 1000.f);
        #else
        sprintf(buf, "%2dfps %5.2fms", (int)fps, (now - prev_frame) * 1000.f);
        #endif
        SDL_SetWindowTitle(window, buf + (buf[0] == ' '));
    }
    ++frames;
    prev_frame = now;
}

double *window_get( int variable ) {
    static double r[2] = {0};

    switch( variable ) {
        break; default:
        break; case WINDOW_WIDTH:
            r[0] = window_size()[0];
            return r;
        break; case WINDOW_HEIGHT:;
            r[0] = window_size()[1];
            return r;
    }
    return r[0] = 0, r;
}

#endif
