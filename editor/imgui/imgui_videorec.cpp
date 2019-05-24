#include <stdio.h>
#include <stdarg.h>
#include "3rd/@jonolick/jo_mpeg.cpp"
#include "3rd/@jonolick/jo_gif.cpp"

#include <omp.h>
#define app_ms() (uint64_t)omp_get_wtime()

static double show_wireframe = 0, show_textures = 0;
static FILE *rec0 = 0, *rec1 = 0;
static jo_gif_t gif0, gif1;


double *get_render( int cmd ) {
    if( cmd == 'wire' ) return &show_wireframe;
    if( cmd == 'texs' ) return &show_textures;
    if( cmd == 'rec0' ) return (double *)rec0;
    if( cmd == 'rec1' ) return (double *)rec1;
    return 0;
}

void set_render( int cmd, ... ) { //$
    va_list vl;
    va_start(vl, cmd);

    int recording;
    switch( cmd ) {
        default:
        break; case 'wire':  show_wireframe = va_arg(vl, double);
        break; case 'texs':  show_textures = va_arg(vl, double);
        break; case 'rec0':
            recording = (int)va_arg(vl, double);
            if( recording && !rec0 ) rec0 = fopen("rec0.mpg", "wb");
            else if( rec0 && !recording ) fclose(rec0), rec0 = 0;
            if( rec0 ) setvbuf( rec0, NULL, _IOFBF, 4 * 1024 * 1024 );
        break; case 'rec1':
            recording = (int)va_arg(vl, double);
            if( recording && !rec1 ) rec1 = fopen("rec1.mpg", "wb");
            else if( rec1 && !recording ) fclose(rec1), rec1 = 0;
            if( rec1 ) setvbuf( rec1, NULL, _IOFBF, 4 * 1024 * 1024 );
/*
            if( recording )  {
                gif1 = jo_gif_start("rec1.gif", (short)*app('w'), (short)*app('h'), 0, 255);
            }
            else if( !recording ) jo_gif_end(&gif1);
*/
    }

    va_end(vl);
}



double *get_app( int var ) {
    static double rect[] = { 640, 480 };
    if( var == 'w' ) return rect;
    if( var == 'h' ) return rect+1;
    static double rate[] = { 60 };
    if( var == 'rate' ) return rate;
    static double empty[] = {0,0,0,0};
    return empty;
}

void set_app( int cmd, ... ) { //$
    va_list vl;
    va_start(vl, cmd);

    double *v = get_app( cmd );
    if( cmd == 'rate' || cmd == 'w' || cmd == 'h' ) *v = va_arg(vl, double);

    va_end(vl);
}



void* screenshot(void **pixels, int w, int h, int fmt) {
    if( fmt == 'rgb' || fmt == 'bgr' ) {
        // sync
        *pixels = (uint8_t*)realloc( *pixels, w * h * 3 );
        if( !*pixels ) return 0;

        // 10 ms, sync
        // copy pixels from screen
        //glBindTexture(GL_TEXTURE_2D, screenTex);
        //glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, w, h);
        GL( glPixelStorei(GL_PACK_ALIGNMENT, 1) );
        GL( glReadPixels(0, 0, w, h, fmt == 'rgb' ? GL_RGB : GL_BGR, GL_UNSIGNED_BYTE, (GLvoid *)*pixels) );
        GL( glPixelStorei(GL_UNPACK_ALIGNMENT, 1) );
    } else {
        // async
        static GLuint pbo = 0;
        if( !pbo ) {
            GL( glGenBuffers(1, &pbo) );
            GL( glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo) );
            GL( glBufferData(GL_PIXEL_PACK_BUFFER, w * h * 4, NULL, GL_STATIC_READ) );
        }

        // for each read back, be sure the right PBO is bound
        GL( glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo) );
        GL( glReadPixels(0, 0, w, h, fmt == 'rgba' ? GL_RGBA : GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 0) ); // BGR/A might be faster

        GL( *pixels = (GLubyte *)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY) );
    }

    // invert pixels (from SOILs source code)
    /*
    image img;
    img.w = w;
    img.h = h;
    img.channels = fmt == 'rgb' || fmt == 'bgr' ? 3 : 4;
    img.deleter = 0;
    img.pixels = pixels;
    image_flip( pixels );
    */

    return *pixels;
}

void freeshot(void **pixels, int fmt) {
    if( fmt == 'bgr' || fmt == 'rgb' ) {
        free(*pixels), *pixels = 0;
    } else {
        if (!glUnmapBuffer(GL_PIXEL_PACK_BUFFER)) {
               // Handle error case
        }
    }
}

// ----------------------------------------------------------------------------


static void mpeg(FILE *rec) {
    int w = (int)get_app('w')[0], h = (int)get_app('h')[0];
    static void *shot = 0;
    screenshot( &shot, w, h, 'bgra' );
    jo_write_mpeg( rec, (const unsigned char *)shot, w, h, 24 ); // (int)get_app('rate')[0] );
    freeshot( &shot, 'bgra' );
}
static void gif(jo_gif_t *rec) {
    int w = (int)get_app('w')[0], h = (int)get_app('h')[0];
    static void *shot = 0;
    screenshot( &shot, w, h, 'rgba' );
    jo_gif_frame( rec, (unsigned char *)shot, 1, 0 );
    freeshot( &shot, 'rgba' );
}



int on_predraw() {
    if( show_wireframe == 1 ) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return 0;
}

int on_postdraw() {
    if( show_wireframe == 1 ) glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    if( rec0 ) {
        static uint64_t frame = 0, then = 0;
        if( 1 ) {
        // if( ++frame >= (get_app('rate')[0]/24) ) {
        // if( (app_ms() - then) >= (1000/24) ) {
            frame = 0;
            mpeg(rec0);
            //gif(&gif0);
            then = app_ms();
        }
    }
    return 0;
}

int on_preswap() {
    if( rec1 ) {
        static uint64_t frame = 0, then = 0;
        if( 1 ) {
        // if( ++frame >= (get_app('rate')[0]/24) ) {
        // if( (app_ms() - then) >= (1000/24) ) {
            frame = 0;
            mpeg(rec1);
            //gif(&gif1);
            then = app_ms();
        }
    }
    return 0;
}
