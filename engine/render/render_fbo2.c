// fbo, render to texture. based on code by @evanw (CC0). see: https://github.com/evanw/gl4
// - rlyeh, public domain.

#ifndef FBO2_H
#define FBO2_H

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
    FBO_DONT_AUTODEPTH = 1,
    FBO_DONT_AUTORESIZE = 2,
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
API void fbo_create(fbo*, int flags);
API void fbo_destroy(fbo*);

// Draw to texture 2D in the indicated attachment location (or a 2D layer of a 3D texture).
// Stop drawing to the indicated color attachment
API void fbo_attach_color(fbo*, texture2 t, unsigned attachment /*= 0*/, unsigned layer3d /*= 0*/);
API void fbo_detach_color(fbo*, unsigned attachment /*= 0*/);
API void fbo_check(fbo*);

// Draw calls between these will be drawn to attachments. If resizeViewport is true this will automatically resize the viewport to the size of the last attached texture.
API void fbo_bind(fbo*);
API void fbo_unbind(fbo*);

// ---

API unsigned fbo2( texture3 color, texture3 depth );
API void     fbo2_bind(unsigned id);
API void     fbo2_unbind();



#endif
#ifdef FBO2_C
#pragma once


void fbo_create(fbo *f, int flags) {
    fbo i = {0};
    i.autoDepth = flags & FBO_DONT_AUTODEPTH ? 0 : 1;
    i.resizeViewport = flags & FBO_DONT_AUTORESIZE ? 0 : 1;
    glGenFramebuffers(1, &i.id);
    *f = i;
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

void fbo_attach_color(fbo *f, texture2 texture, unsigned attachment, unsigned layer3d) {
    f->newViewport[2] = texture.width;
    f->newViewport[3] = texture.height;

    fbo_bind(f);

    // Bind a 2D texture (using a 2D layer of a 3D texture)
    if (texture.target == GL_TEXTURE_2D) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, texture.target, texture.id, 0);
    } else {
        glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, texture.target, texture.id, 0, layer3d);
    }

    // Need to call glDrawBuffers() for OpenGL to draw to multiple attachments
    if (attachment >= array_count(f->drawBuffers)) {
        array_push(f->drawBuffers, GL_NONE);
    }
    f->drawBuffers[attachment] = GL_COLOR_ATTACHMENT0 + attachment;
    glDrawBuffers(array_count(f->drawBuffers), f->drawBuffers);

    fbo_unbind(f);
}

void fbo_check(fbo *f) {
    fbo_bind(f);

    // fbo_check
    if (f->autoDepth) {
        if (f->renderbufferWidth != f->newViewport[2] || f->renderbufferHeight != f->newViewport[3]) {
            f->renderbufferWidth = f->newViewport[2];
            f->renderbufferHeight = f->newViewport[3];
            #if 1
            if(!f->renderbuffer) glGenRenderbuffers(1, &f->renderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, f->renderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24/*32*/, f->renderbufferWidth, f->renderbufferHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            #endif
        }
        #if 1
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, f->renderbuffer);
        #endif
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

// ---

unsigned fbo2( texture3 color, texture3 depth ) {
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // color
    if( color.id ) {
        glBindTexture(GL_TEXTURE_2D, color.id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.id, 0);
    }

    // depth
    if( depth.id ) {
        // https://community.khronos.org/t/rendering-the-depth-buffer-to-a-texture-in-a-fbo/64739/5
        glBindTexture(GL_TEXTURE_2D, depth.id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.id, 0);
    } else {
        // create a renderbuffer object for depth and stencil attachment (you cant sample this)
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, color.width, color.height); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    }

    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        case GL_FRAMEBUFFER_COMPLETE: break;
        case GL_FRAMEBUFFER_UNDEFINED: assert(!"GL_FRAMEBUFFER_UNDEFINED");
        case GL_FRAMEBUFFER_UNSUPPORTED: assert(!"GL_FRAMEBUFFER_UNSUPPORTED");
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: assert(!"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: assert(!"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: assert(!"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: assert(!"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: assert(!"GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: assert(!"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: assert(!"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: assert(!"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        default: assert(!"Unknown glCheckFramebufferStatus error");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return framebuffer;
}

void fbo2_bind(unsigned id) {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}
void fbo2_unbind() {
    fbo2_bind(0);
}

#endif
