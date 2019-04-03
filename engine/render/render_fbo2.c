// fbo, render to texture. based on code by @evanw (CC0). see: https://github.com/evanw/gl4
// - rlyeh, public domain.

#ifndef FBO_H
#define FBO_H
#include <engine.h>


// A framebuffer object that can take color attachments. Draw calls between
// bind() and unbind() are drawn to the attached textures.
//
// Usage:
//
//     fbo f; fbo_create(&f, 0);
//     fbo_attach_color(&f, texture, 0,0);
//
//     fbo_bind(&f);
//     // draw stuff
//     fbo_unbind(&f);
//

enum fbo_flags {
    FBO_DONT_AUTODEPTH,
    FBO_DONT_AUTORESIZE,
};

typedef struct fbo {
    unsigned id;
    unsigned renderbuffer;
    bool autoDepth;
    bool resizeViewport;
    int newViewport[4], oldViewport[4];
    int renderbufferWidth, renderbufferHeight;
    array(unsigned) drawBuffers;
} fbo;

// 
// 
API fbo* fbo_create(fbo*, int flags);
API void fbo_destroy(fbo*);

// Draw to texture 2D in the indicated attachment location (or a 2D layer of a 3D texture).
// Stop drawing to the indicated color attachment
API void fbo_attach_color(fbo*, texture2 t, unsigned attachment /*= 0*/, unsigned layer /*= 0*/);
API void fbo_detach_color(fbo*, unsigned attachment /*= 0*/);

// Draw calls between these will be drawn to attachments. If resizeViewport is true this will automatically resize the viewport to the size of the last attached texture.
API void fbo_bind(fbo*);
API void fbo_unbind(fbo*);


#endif
#ifdef FBO_C
#pragma once


void fbo_create(fbo *f, int flags) {
    fbo i = {0};
    i.autoDepth = flags & FBO_DONT_AUTODEPTH ? 0 : 1;
    i.resizeViewport = flags & FBO_DONT_AUTORESIZE ? 0 : 1;
    glGenFramebuffers(1, &i.id);
    glGenRenderbuffers(1, &i.renderbuffer);
    *f = i;
    return f;
}

void fbo_destroy(fbo *f) {
    glDeleteRenderbuffers(1, &f->renderbuffer);
    glDeleteFramebuffers(1, &f->id);

    fbo c = {0};
    *f = c;
}

void fbo_bind(fbo *f) {
    glBindFramebuffer(GL_FRAMEBUFFER, f->id);
    if (f->resizeViewport) {
        glGetIntegerv(GL_VIEWPORT, f->oldViewport);
        glViewport(f->newViewport[0], f->newViewport[1], f->newViewport[2], f->newViewport[3]);
    }
}

void fbo_unbind(fbo *f) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (f->resizeViewport) {
        glViewport(f->oldViewport[0], f->oldViewport[1], f->oldViewport[2], f->oldViewport[3]);
    }
}

void fbo_attach_color(fbo *f, texture2 texture, unsigned attachment, unsigned layer) {
    f->newViewport[2] = texture.width;
    f->newViewport[3] = texture.height;

    fbo_bind(f);

    // Bind a 2D texture (using a 2D layer of a 3D texture)
    if (texture.target == GL_TEXTURE_2D) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, texture.target, texture.id, 0);
    } else {
        glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, texture.target, texture.id, 0, layer);
    }

    // Need to call glDrawBuffers() for OpenGL to draw to multiple attachments
    if (attachment >= array_count(f->drawBuffers)) {
        array_push(f->drawBuffers, GL_NONE);
    }
    f->drawBuffers[attachment] = GL_COLOR_ATTACHMENT0 + attachment;
    glDrawBuffers(array_count(f->drawBuffers), f->drawBuffers);

    // fbo_check
    if (f->autoDepth) {
        if (f->renderbufferWidth != f->newViewport[2] || f->renderbufferHeight != f->newViewport[3]) {
            f->renderbufferWidth = f->newViewport[2];
            f->renderbufferHeight = f->newViewport[3];
            glBindRenderbuffer(GL_RENDERBUFFER, f->renderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24/*32*/, f->renderbufferWidth, f->renderbufferHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, f->renderbuffer);
    }
    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        case GL_FRAMEBUFFER_COMPLETE: break;
        case GL_FRAMEBUFFER_UNDEFINED: assert(!"GL_FRAMEBUFFER_UNDEFINED\n");
        case GL_FRAMEBUFFER_UNSUPPORTED: assert(!"GL_FRAMEBUFFER_UNSUPPORTED\n");
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: assert(!"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: assert(!"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: assert(!"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: assert(!"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n");
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: assert(!"GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n");
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: assert(!"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n");
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: assert(!"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: assert(!"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
        default: assert(!"Unknown glCheckFramebufferStatus error");
    }

    fbo_unbind(f);
}

void fbo_detach_color(fbo *f, unsigned attachment) {
    fbo_bind(f);

    // Update the draw buffers
    if (attachment < array_count(f->drawBuffers)) {
        f->drawBuffers[attachment] = GL_NONE;
        glDrawBuffers(array_count(f->drawBuffers), f->drawBuffers);
    }

    fbo_unbind(f);
}

#endif
