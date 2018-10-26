#ifndef TEXTURE_H
#define TEXTURE_H

#include "../media/image/image.c"

enum FLAGS {
    FLAG_00 = 1<<0,
    FLAG_01 = 1<<1,
    FLAG_02 = 1<<2,
    FLAG_03 = 1<<3,
    FLAG_04 = 1<<4,
    FLAG_05 = 1<<5,
    FLAG_06 = 1<<6,
    FLAG_07 = 1<<7,
    FLAG_08 = 1<<8,
    FLAG_09 = 1<<9,
    FLAG_10 = 1<<10,
    FLAG_11 = 1<<11,
    FLAG_12 = 1<<12,
    FLAG_13 = 1<<13,
    FLAG_14 = 1<<14,
    FLAG_15 = 1<<15,
    FLAG_16 = 1<<16,
    FLAG_17 = 1<<17,
};


enum {
    TEXTURE_REPEAT_X = FLAG_00,
    TEXTURE_REPEAT_Y = FLAG_01,
    TEXTURE_REPEAT_Z = FLAG_02,

    TEXTURE_ANISOTROPY = FLAG_03,
    TEXTURE_PREMULTIPLY = FLAG_04,
    TEXTURE_FLIP = FLAG_05,

    TEXTURE_RGB = FLAG_06,
    TEXTURE_RGBA = FLAG_07,

    // NEAREST as POINT intead?

    // minification filters (when projected texel is smaller than a pixel)
    TEXTURE_MIN_NEAREST = FLAG_08,          // no filtering
    TEXTURE_MIP_NEAREST_NEAREST = FLAG_09,  // no filtering, but mipmap selection
    TEXTURE_MIP_NEAREST_LINEAR = FLAG_10,   // linear filtering between mipmap levels, but no filtering inside each mipmap level
    TEXTURE_MIN_LINEAR = FLAG_11,           // bilinear filtering
    TEXTURE_MIP_LINEAR_NEAREST = FLAG_12,   // bilinear filtering, with mipmap selection
    TEXTURE_MIP_LINEAR_LINEAR = FLAG_13,    // trilinear filtering

    // magnification filters (when projected texel is bigger than a pixel)
    TEXTURE_MAG_NEAREST = FLAG_14,
    TEXTURE_MAG_LINEAR = FLAG_15,

    TEXTURE_REPEAT = TEXTURE_REPEAT_X | TEXTURE_REPEAT_Y | TEXTURE_REPEAT_Z,
    TEXTURE_NEAREST = TEXTURE_MIN_NEAREST | TEXTURE_MAG_NEAREST,
    TEXTURE_MIPMAPS = TEXTURE_MIP_LINEAR_NEAREST,
};

// TEXTURE_R, TEXTURE_RG, TEXTURE_RGB, TEXTURE_RGBA,
// TEXTURE_8, TEXTURE_F,
// TEXTURE_RAW, TEXTURE_ETC1, TEXTURE_ETC2, TEXTURE_DXT, TEXTURE_PVR, TEXTURE_PVR2,
// etc


API unsigned texture(const char *pathfile, int flags);
API unsigned texture_mem(image img, int flags);

API int texture_width( unsigned id );
API int texture_height( unsigned id );

API void texture_bind( unsigned id, unsigned unit );
API void texture_destroy( unsigned *id );


#endif

// ----------------------------------------------------------------------------

#ifdef TEXTURE_C
#pragma once
#include "render_renderer.c"

/*static*/ unsigned texture_count = 0;
/*static*/ unsigned texture_budget = 0;

typedef struct texture_internal {
    image img;
} texture_internal;

static texture_internal *i = 0;
static int i_count = 0;

unsigned texture_mem(image img, int flags) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Table1. GL_RED, GL_RG, GL_RGB, GL_RGBA
    // Table2. GL_R8, GL_R8_SNORM, GL_R16, GL_R16_SNORM, GL_RG8, GL_RG8_SNORM, GL_RG16, GL_RG16_SNORM, GL_R3_G3_B2, GL_RGB4, GL_RGB5, GL_RGB8, GL_RGB8_SNORM, GL_RGB10, GL_RGB12, GL_RGB16_SNORM, GL_RGBA2, GL_RGBA4, GL_RGB5_A1, GL_RGBA8, GL_RGBA8_SNORM, GL_RGB10_A2, GL_RGB10_A2UI, GL_RGBA12, GL_RGBA16, GL_SRGB8, GL_SRGB8_ALPHA8, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F, GL_R11F_G11F_B10F, GL_RGB9_E5, GL_R8I, GL_R8UI, GL_R16I, GL_R16UI, GL_R32I, GL_R32UI, GL_RG8I, GL_RG8UI, GL_RG16I, GL_RG16UI, GL_RG32I, GL_RG32UI, GL_RGB8I, GL_RGB8UI, GL_RGB16I, GL_RGB16UI, GL_RGB32I, GL_RGB32UI, GL_RGBA8I, GL_RGBA8UI, GL_RGBA16I, GL_RGBA16UI, GL_RGBA32I, GL_RGBA32UI, 
    // Table3. GL_COMPRESSED_RED, GL_COMPRESSED_RG, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, GL_COMPRESSED_SRGB, GL_COMPRESSED_SRGB_ALPHA, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_SIGNED_RG_RGTC2, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, 

    GLenum tex_format = GL_RGBA; 
    GLenum img_format = img.channels <= 1 ? GL_RED : img.channels <= 2 ? GL_RG : img.channels <= 3 ? GL_RGB /*GL_BGR*/ : GL_RGBA /*GL_BGRA*/;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, tex_format, img.w, img.h, 0, img_format, GL_UNSIGNED_BYTE, img.pixels);

    int min_filter = flags & TEXTURE_MIN_NEAREST ? GL_NEAREST : GL_LINEAR;
    int mag_filter = flags & TEXTURE_MAG_NEAREST ? GL_NEAREST : GL_LINEAR;

    /**/ if( flags & TEXTURE_MIP_NEAREST_NEAREST ) min_filter = GL_NEAREST_MIPMAP_NEAREST;
    else if( flags & TEXTURE_MIP_NEAREST_LINEAR  ) min_filter = GL_NEAREST_MIPMAP_LINEAR;
    else if( flags & TEXTURE_MIP_LINEAR_NEAREST  ) min_filter = GL_LINEAR_MIPMAP_NEAREST;
    else if( flags & TEXTURE_MIP_LINEAR_LINEAR   ) min_filter = GL_LINEAR_MIPMAP_LINEAR;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    // also
    // GL_MIRRORED_REPEAT (tileable repeat)
    // GL_CLAMP_TO_BORDER (black border only if:)
    // also
    // float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, flags & TEXTURE_REPEAT_X ? GL_REPEAT : GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, flags & TEXTURE_REPEAT_Y ? GL_REPEAT : GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, flags & TEXTURE_REPEAT_Z ? GL_REPEAT : GL_CLAMP_TO_EDGE );

    if( flags & TEXTURE_ANISOTROPY ) {
        enum {
            TEXTURE_MAX_ANISOTROPY_EXT = 0x84FE,
            MAX_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FF
        };
        // [1.0 .. 16.0] 1f is off, 8f for old hw, 16f max modern
        GLfloat largest;
        glGetFloatv(/*GL_*/MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
        glTexParameterf(GL_TEXTURE_2D, /*GL_*/TEXTURE_MAX_ANISOTROPY_EXT, largest);
    }
    
    if( flags & TEXTURE_MIPMAPS ) {
        //printf("%p\n", glGenerateMipmap);
        if( glGenerateMipmap ) {
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
        }
    }

    // store details in cache
    i = (texture_internal*)realloc( i, sizeof(texture_internal) * (id+1) /*++i_count*/ );
    i[id /*i_count - 1*/].img = img;

    texture_count++;
    texture_budget += img.w * img.h * (img.channels <= 1 ? 1 : img.channels);

    image_destroy(&img);
    return id;
}

unsigned texture(const char *pathfile, int texflags) {
    image img;
    int imgflags = 0
        | (texflags & TEXTURE_RGB  ? IMAGE_RGB   : 0)
        | (texflags & TEXTURE_RGBA ? IMAGE_RGBA  : 0)
        | (texflags & TEXTURE_FLIP ? IMAGE_FLIPH : 0);
    if( !image_loadfile(&img, pathfile, imgflags) ) {
        return 0;
    }
    return texture_mem( img, texflags );
}

int texture_width( unsigned id ) {
    return i[id].img.w;
}

int texture_height( unsigned id ) {
    return i[id].img.h;
}

void texture_bind( unsigned id, unsigned unit ) {
    if( id ) {
        //glEnable( GL_TEXTURE_2D );
        glActiveTexture( GL_TEXTURE0 + unit );
        glBindTexture( GL_TEXTURE_2D, id );
    }
}

void texture_destroy(unsigned *tex) {
    if( tex && *((GLuint*)tex) > 0 ) {
        glDeleteTextures(1, (GLuint *)tex);
        // @todo: remove texture from images[] and textures[], also stats
        *tex = 0;
    }
}

int texture_placeholder() {
    // generate blank texture
    unsigned char header[ 128 + 32 * 32 * 3 ] = "P6\n32 32\n255\n";
    unsigned char *buffer = &header[ strlen(header) ];

    for( int y = 0; y < 32; y++ ) {
        for( int x = 0; x < 32; x++ ) {
            int c = ( x ^ y ) % 2;
            *buffer++ = c ? 255 : 0;
            *buffer++ = c ?   0 : 255;
            *buffer++ = c ? 255 : 255;
        }
    }

    image img;
    img.w = 32;
    img.h = 32;
    img.channels = 3;
    img.format = 0;
    img.pixels = header;
    img.deleter = 0;

    int checker = texture_mem( img, /*sizeof(image),*/ 0 );
    return checker;
}

#endif
