// Image loading and saving: for JPG and PNG

/*
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"
*/

#ifndef IMAGE_H
#define IMAGE_H
#include <stdbool.h>
#include <stdint.h>

enum {
    // channels
    IMAGE_ANY = 0,
    IMAGE_R = 1,
    IMAGE_G = 2,
    IMAGE_B = 4,
    IMAGE_A = 8,
    IMAGE_RGB = IMAGE_R|IMAGE_G|IMAGE_B,
    IMAGE_RGBA = IMAGE_RGB|IMAGE_A,
    // format
    IMAGE_U8 = 16,
    IMAGE_U16 = 32,
    IMAGE_F32 = 64,
    // utils
    IMAGE_FLIPW = 128,
    IMAGE_FLIPH = 256,
};

typedef struct image {
    int w, h, channels;
    int format;
    void *pixels;
    void (*deleter)(void *);
} image;

API bool image_loadfile( image *, const char *pathfile, int flags );
API bool image_loadmem( image *img, const void *src, int srclen, int flags );
API bool image_destroy( image * );

// @todo: test me
API void image_bgr2rgb( image * );
API void image_mirror( image * );
API void image_flip( image * );

#endif








#ifdef IMAGE_C
#pragma once
#include "engine.h" // filesys
#define swapifbe(x) (x) // endian.h

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "3rd/stb_image.h"
#include "3rd/stb_image_write.h"
#include "3rd/stb_image_resize.h"

// @todo
// typedef bool image_decoder( image *img, const void *ptr, size_t size, int flags );
// image_decoder extra_decoders[256] = {0}, default_decoder = 0;

bool image_loadmem( image *img, const void *src, int srclen, int flags ) {
    int x, y, n;
    int channels = 0;
    channels += !!(flags & IMAGE_R);
    channels += !!(flags & IMAGE_G);
    channels += !!(flags & IMAGE_B);
    channels += !!(flags & IMAGE_A);
    stbi_set_flip_vertically_on_load(flags & IMAGE_FLIPH ? 1 : 0);
    stbi_uc *data = stbi_load_from_memory(src, srclen, &x, &y, &n, channels); // also, stbi_loadf ; also, channels can be 0
    if(!data) {
        return false;
    }
    img->w = x;
    img->h = y;
    img->channels = channels ? channels : n;
    img->pixels = data;
    img->deleter = stbi_image_free;
    return true;
}

static
bool pug_decoder( image *img, const void *ptr, size_t size, int flags ) {
    // if .pug file, then decode alpha (png) and blend it over regular lossy image (jpg)
    if( 0 == memcmp( "pug1", (const char *)ptr + size - 4, 4 ) ) {
        const int32_t color_size = swapifbe( *(const int32_t *)((const char *)ptr + size - 12) );
        const int32_t alpha_size = swapifbe( *(const int32_t *)((const char *)ptr + size - 8) );
        int w2 = 0, h2 = 0, bpp2 = 0;
        stbi_uc *alpha = stbi_load_from_memory( (const unsigned char *)ptr + color_size, alpha_size, &w2, &h2, &bpp2, 1 );
        if( alpha ) {
            bool ok = image_loadmem( img, ptr, color_size, flags | IMAGE_RGBA );
            if( ok ) {
                for( unsigned it = 0, end = w2 * h2; it < end; ++it ) {
                    ((uint8_t*)img->pixels)[ it * 4 + 3 ] = alpha[ it ];
                }
            }
            stbi_image_free( alpha );
            return true;
        }
    }
    return false;
}

bool image_loadfile( image *img, const char *pathfile, int flags ) {
    void *ptr = file_read_(pathfile);
    int size = file_size_(pathfile);
    bool ok = 0;
    if( !ok ) {
        ok = pug_decoder( img, ptr, size, flags );
    }
    if( !ok ) {
        ok = image_loadmem( img, ptr, size, flags );
    }
    if( !ok ) {
        printf("Error reading image data: %s (%d)\n", pathfile, img->channels);
    } else {
        printf("%s %dx%dx%d\n", pathfile, img->w, img->h, img->channels);
    }
    return ok;
}

bool image_destroy( image *img ) {
    if( img->deleter ) img->deleter( img->pixels );
    return true;
}

// @todo: test me
void image_bgr2rgb( image *img ) {
    unsigned char *data = (unsigned char *)img->pixels, *end = data + img->w * img->h * img->channels;
    while( data < end ) {
        unsigned char swap = data[0];
        data[0] = data[2];
        data[2] = swap;
        data += img->channels;
    }
}

// @todo: optimize this. sub-optimal.
void image_mirror( image *img ) {
    int w = img->w, h = img->h, ch = img->channels;
    unsigned char *rgb = (unsigned char *)img->pixels;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w/2; ++x) {
            for( int c = 0; c < ch; ++c ) {
                int src = (x + y * w) * ch;
                int dst = (((w-1)-x) + y * w) * ch;
                uint8_t tmp = rgb[src+c];
                rgb[src+c] = rgb[dst+c];
                rgb[dst+c] = tmp;
            }
        }
    }
}

// invert pixels, 9 ms
// from SOIL source code: public domain
void image_flip( image *img ) {
    int w = img->w, h = img->h, ch = img->channels;
    uint8_t *color = (uint8_t*)img->pixels;
    for (int j = 0; j * 2 < h; ++j) {
        int x = j * w * ch;
        int y = (h - 1 - j) * w * ch;
        for (int i = w * ch; i > 0; --i) {
            uint8_t temp = color[x];
            color[x] = color[y];
            color[y] = temp;
            ++x;
            ++y;
        }
    }
}

#endif
