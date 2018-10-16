#ifndef WINDOW_H
#define WINDOW_H

#include "../render/render.c" // opengl
#include "3rd/GLFW/glfw3.h"

extern GLFWwindow *window;

enum {
    WINDOW_LEGACY_OPENGL = 0x4,
};

API int window_create( float zoom /* 10.0f */, int flags );
API int window_update( int *w, int *h );
API void window_swap( void **pixels );
API void window_destroy();

enum {
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
};

API double *window_get( int variable );


#endif


#ifdef WINDOW_C
#pragma once
#include "../input/input.c"
#include "render.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// extern const char *callstack(int traces);
// const char * (*callstack_handler)(int traces) = 0; // = callstack;
int (*printf_handler)(const char *fmt, ...) = printf;

static void die_callback( const char *text ) {
    fprintf(stderr, "%s\n", text);
    exit(-1);
}
static void error_callback(int error, const char* description) {
    int whitelisted = !!strstr(description, "Failed to create OpenGL context");
    if(whitelisted) return;
    fprintf(stderr, "GLFW error %#x: %s\n", error, description);
}
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


/*static*/ GLFWwindow *window = 0;
void window_destroy(void) {
    render_quit();
    if(window) {
        glfwDestroyWindow(window);
        window = 0;
    }
    // if( num_active_windows == 0 )
    glfwTerminate(); // exit(0)
}
int window_create( float zoom, int flags ) {
    if( window ) {
        return 0;
    } else {
        glfwSetErrorCallback(error_callback);
        if( !glfwInit() ) {
            die_callback("cant init glfw");
        }
        //atexit(window_destroy);
    }

    const char *title = "";
#ifdef _MSC_VER
    int arg0len = strlen( __argv[0] );
    if( arg0len > 4 ) {
        char *dot = &__argv[0][arg0len - 4];
        if( 0 == strcmp( dot, ".exe" ) || 0 == strcmp( dot, ".EXE" ) ) {
            *dot = '\0';
        }
    }
    title = __argv[0];
#endif

    // window screen coverage (zoom)
    // zoom = 1..100%, or [0.f .. 1.f] ; fullscreen if zoom = 100% or 1.f ; borderless-fullscreen if zoom > 100% (and also if 1.1f or negative zoom)
    // @todo: remove [0..1] range?
    int fullscreen = 0;
    zoom = zoom > 1.1 ? zoom / 100.f : zoom;
    zoom = zoom > 0 && zoom < 1 ? zoom : (fullscreen = 1, 1);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* desktop = glfwGetVideoMode(primary);
    int appw = (int)(desktop->width * zoom), apph = (int)(desktop->height * zoom);
    int w = appw, h = apph;

    int have_debug = GL_FALSE;
    int have_core = flags & WINDOW_LEGACY_OPENGL ? GLFW_OPENGL_COMPAT_PROFILE : GLFW_OPENGL_CORE_PROFILE;

    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // glfwWindowHint(GLFW_RED_BITS, desktop->redBits);
    // glfwWindowHint(GLFW_GREEN_BITS, desktop->greenBits);
    // glfwWindowHint(GLFW_BLUE_BITS, desktop->blueBits);
    // glfwWindowHint(GLFW_REFRESH_RATE, desktop->refreshRate);
    // glfwWindowHint(GLFW_STENCIL_BITS, 32 /*GLFW_DONT_CARE*/);
//    glfwWindowHint(GLFW_DEPTH_BITS, 0);
//    glfwWindowHint(GLFW_STENCIL_BITS, 0);
//    glfwWindowHint(GLFW_SAMPLES, 0);

    // try core first, unless compat stated
    int majors[] = { 4, 4, 4, 4, 4, 3, 3 };
    int minors[] = { 4, 3, 2, 1, 0, 3, 2 };
    for( int i = 0; i < sizeof(majors) / sizeof(majors[0]); ++i ) {
        int majv = majors[i], minv = minors[i];
        if( !window && !(flags & WINDOW_LEGACY_OPENGL) ) {
            // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majv);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minv);
            glfwWindowHint(GLFW_OPENGL_PROFILE, have_core );
        #if 1 // !SHIPPING
            // only in +4.3
            // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, have_debug = GL_TRUE);
        #endif

            if( fullscreen )
                window = glfwCreateWindow(desktop->width, desktop->height, title, primary, NULL);
            else
                window = glfwCreateWindow(appw, apph, title, NULL, NULL);

            if(window) printf("opengl %d.%d%s%s context created\n", majv, minv, have_core ? "-core" : "", have_debug ? "-debug" : "");
        }
    }

    // else 2.1 compat
    if( !window ) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE ); // GLFW_OPENGL_COMPAT_PROFILE );
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, have_debug = GL_FALSE);

        if( fullscreen )
            window = glfwCreateWindow(desktop->width, desktop->height, title, primary, NULL);
        else
            window = glfwCreateWindow(appw, apph, title, NULL, NULL);

        if(window) printf("opengl 2.1%s%s context created\n", 0 ? "-core" : "", have_debug ? "-debug" : "");
    }

    if (!window) {
        die_callback("cant create window");
    }

    // center
    if( desktop ) {
        glfwSetWindowPos(window , (desktop->width-appw)/2 , (desktop->height-apph)/2);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
#  if defined __gl3w_h_
    gl3w_init();
#elif defined __glad_h_
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddressExtraCompat);
#else
    puts("warning: no opengl loader found");
#endif

    // intel gma hd
    if(!glGenVertexArrays) glGenVertexArrays = (void*)glfwGetProcAddressExtraCompat("glGenVertexArrays");
    if(!glBindVertexArray) glBindVertexArray = (void*)glfwGetProcAddressExtraCompat("glBindVertexArray");

    // Enable the debug callback
    if( have_debug ) {
        trap_gl();
    }

    glfwSwapInterval(1); // also check -1 

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    render_init();

    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);
    //glGetIntegerv(GL_MAX_SAMPLES, &max_supported_samples);

    return 1;
}
int window_update(int *w, int *h) {
    if( glfwWindowShouldClose(window) ) {
        return 0;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float ratio = width / (height + 1.f);
    render_update(width, height);
    mouse_update();
    if(w) *w = width;
    if(h) *h = height;
    return 1;
}

void render_capture( int w, int h, int comp, void *pixels );

void window_swap( void **pixels ) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    render_post(w, h);

    glfwSwapBuffers(window);
    glfwPollEvents();

    //    djgt_save_glcolorbuffer_png(GL_COLOR_ATTACHMENT0, GL_RGB, "out.png");
    if( pixels && !glfwWindowShouldClose(window) ) {
        *pixels = (unsigned char *)realloc(*pixels, 4 * w * h);
        render_capture(w,h,3,*pixels);
    }
}


double *window_get( int variable ) {
    static double r[2] = {0};
    int w, h;
    switch( variable ) {
        break; default:
        break; case WINDOW_WIDTH:
            glfwGetFramebufferSize(window, &w, &h);
            r[0] = w;
            return r;
        break; case WINDOW_HEIGHT:;
            glfwGetFramebufferSize(window, &w, &h);
            r[0] = h;
            return r;
    }
    return r[0] = 0, r;
}

#endif
