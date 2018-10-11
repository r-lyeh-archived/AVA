/* proposal:
    int font_id = font("Roboto-Regular.ttf");
    // [...]

    textFont(font_id);
    textAlign(ALIGN_CENTER);
    textSize(30);
    textLeading(5);
    text("test everything here", width/2, height/2);
*/

// instantiate text mesh

void text(void *renderable, int font_id, const char *text );


#ifdef RENDER_C
#pragma once
#include "render_renderer.c"
#include "render_font.c"
#include "render_unicode.c"
#include "render_mesh.c" // buffer
#include "engine.h" // realloc


void text(void *renderable, int font_id, const char *text ) {
    renderable_t *r = (renderable_t*)renderable;
    font_t *f = &fonts[ font_id ];
if(!f) { /* init on demand */ font_mem(0, 0, 0, 0); f = &fonts[0]; }

    // utf8 to utf32

    uint32_t *text32 = string32( text );
    int count = strlen32( text32 );

    // setup vertex+index data

#if 1
    // 4 verts, 6 edges/2 polys
    int vlen = 4 * count; void *vertices = (void*)malloc( vlen * sizeof(vec3) );
    int ulen = 4 * count; void *uvs      = (void*)malloc( ulen * sizeof(vec2) );
    int clen = 4 * count; void *colors   = (void*)malloc( clen * sizeof(uint32_t) );
    int ilen = 6 * count; void *indexes  = (void*)malloc( ilen * sizeof(uint16_t) );
#else
    static const int k_maxstrlen = 1024;
    static GLfloat vertdata[...];
    static GLuint idxdata[...];
#endif

    struct Vec3  { vec3 v; } *v = (struct Vec3 *)vertices;
    struct Vec2  { vec2 v; } *u = (struct Vec2 *)uvs;
    struct Col32 { uint32_t v; } *c = (struct Col32*)colors;
    struct Idx16 { uint16_t i; } *i = (struct Idx16*)indexes;

    uint16_t lastIndex = 0;
    float offsetX = 0, offsetY = 0;
    for( int s = 0; s < count; ++s ) {
        uint32_t unicode = text32[s] & 0xffff;
        // handle line breaks
        if( unicode == '\r' || unicode == '\n' ) {
            offsetX = 0;
            offsetY += f->spaceY;
            continue;
        }
        // handle tabs
        if( unicode == '\t' ) {
            offsetX += f->spaceX * 4;
            continue;
        }
        // handle regular text
        if( fillglyph( &v->v, &u->v, f, unicode, &offsetX, &offsetY ) ) {
            v += 4;
            u += 4;

#if 0
            c[0].v = 0xFF0000FF; // R
            c[1].v = 0xFF00FF00; // G
            c[2].v = 0xFFFF0000; // B
            c[3].v = 0xFFFFFFFF; // W
#else
            c[0].v = 0xFFFFFFFF; // W
            c[1].v = 0xFFFFFFFF; // W
            c[2].v = 0xFFFFFFFF; // W
            c[3].v = 0xFFFFFFFF; // W
#endif
            c += 4;

            uint16_t idx[] = { 
                lastIndex, lastIndex + 1, lastIndex + 2,
                lastIndex, lastIndex + 2, lastIndex + 3,
            };
            memcpy( i, idx, sizeof(idx) );
            i += 6;

            lastIndex += 4;
        }
    }

    // setup renderable

    buffer buffers[] = {
        {indexes, MSIZE(indexes)},
        {vertices, MSIZE(vertices)},
        {colors, MSIZE(colors)},
        {uvs, MSIZE(uvs)},
    };

    mesh(r, VERTEX_P|VERTEX_C|VERTEX_U | 16, ilen, buffers );

    free(vertices);
    free(colors);
    free(uvs);
    free(indexes);
    free(text32);
}

#endif
