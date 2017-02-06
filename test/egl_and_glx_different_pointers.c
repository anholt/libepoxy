/*
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/**
 * @file egl_and_glx_different_pointers.c
 *
 * Tests that epoxy correctly handles an EGL and GLX implementation
 * that return different function pointers between the two.
 *
 * This is the case for EGL and GLX on nvidia binary drivers
 * currently, but is also the case if someone has nvidia binary GLX
 * installed but still has Mesa (software) EGL installed.  This seems
 * common enough that we should make sure things work.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <dlfcn.h>
#include "epoxy/gl.h"
#include "epoxy/egl.h"
#include "epoxy/glx.h"

#include "egl_common.h"
#include "glx_common.h"
#include "dlwrap.h"

#define GLX_FAKED_VENDOR_STRING "libepoxy override GLX"
#define EGL_FAKED_VENDOR_STRING "libepoxy override EGL"

#define GL_CREATESHADER_VALUE 1234
#define GLES2_CREATESHADER_VALUE 5678

const char *override_GLES2_glGetString(GLenum e);
const char *override_GL_glGetString(GLenum e);
GLuint override_GLES2_glCreateShader(GLenum e);
GLuint override_GL_glCreateShader(GLenum e);

const char *
override_GL_glGetString(GLenum e)
{
    if (e == GL_VENDOR)
        return GLX_FAKED_VENDOR_STRING;

    return DEFER_TO_GL("libGL.so.1", override_GL_glGetString,
                       "glGetString", (e));
}

const char *
override_GLES2_glGetString(GLenum e)
{
    if (e == GL_VENDOR)
        return EGL_FAKED_VENDOR_STRING;

    return DEFER_TO_GL("libGLESv2.so.2", override_GLES2_glGetString,
                       "glGetString", (e));
}

GLuint
override_GL_glCreateShader(GLenum type)
{
    return GL_CREATESHADER_VALUE;
}

GLuint
override_GLES2_glCreateShader(GLenum type)
{
    return GLES2_CREATESHADER_VALUE;
}

#ifdef USE_GLX
static bool
make_glx_current_and_test(Display *dpy, GLXContext ctx, Drawable draw)
{
    const char *string;
    GLuint shader;
    bool pass = true;

    glXMakeCurrent(dpy, draw, ctx);

    if (!epoxy_is_desktop_gl()) {
        fprintf(stderr, "Claimed to be ES\n");
        pass = false;
    }

    string = (const char *)glGetString(GL_VENDOR);
    printf("GLX vendor: %s\n", string);

    shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (shader != GL_CREATESHADER_VALUE) {
        fprintf(stderr, "glCreateShader() returned %d instead of %d\n",
                shader, GL_CREATESHADER_VALUE);
        pass = false;
    }

    pass = pass && !strcmp(string, GLX_FAKED_VENDOR_STRING);

    return pass;
}

static void
init_glx(Display **out_dpy, GLXContext *out_ctx, Drawable *out_draw)
{
    Display *dpy = get_display_or_skip();
    make_glx_context_current_or_skip(dpy);

    *out_dpy = dpy;
    *out_ctx = glXGetCurrentContext();
    *out_draw= glXGetCurrentDrawable();
}
#endif /* USE_GLX */

#ifdef USE_EGL
static bool
make_egl_current_and_test(EGLDisplay *dpy, EGLContext ctx)
{
    const char *string;
    GLuint shader;
    bool pass = true;

    eglMakeCurrent(dpy, NULL, NULL, ctx);

    if (epoxy_is_desktop_gl()) {
        fprintf(stderr, "Claimed to be desktop\n");
        pass = false;
    }

    if (epoxy_gl_version() < 20) {
        fprintf(stderr, "Claimed to be GL version %d\n",
                epoxy_gl_version());
        pass = false;
    }

    shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (shader != GLES2_CREATESHADER_VALUE) {
        fprintf(stderr, "glCreateShader() returned %d instead of %d\n",
                shader, GLES2_CREATESHADER_VALUE);
        pass = false;
    }

    string = (const char *)glGetString(GL_VENDOR);
    printf("EGL vendor: %s\n", string);

    pass = pass && !strcmp(string, EGL_FAKED_VENDOR_STRING);

    return pass;
}

static void
init_egl(EGLDisplay **out_dpy, EGLContext *out_ctx)
{
    EGLDisplay *dpy = get_egl_display_or_skip();
    static const EGLint config_attribs[] = {
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_RED_SIZE, 1,
	EGL_GREEN_SIZE, 1,
	EGL_BLUE_SIZE, 1,
	EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
	EGL_NONE
    };
    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    EGLContext ctx;
    EGLConfig cfg;
    EGLint count;

    if (!epoxy_has_egl_extension(dpy, "EGL_KHR_surfaceless_context"))
        errx(77, "Test requires EGL_KHR_surfaceless_context");

    eglBindAPI(EGL_OPENGL_ES_API);

    if (!eglChooseConfig(dpy, config_attribs, &cfg, 1, &count))
        errx(77, "Couldn't get an EGLConfig\n");

    ctx = eglCreateContext(dpy, cfg, NULL, context_attribs);
    if (!ctx)
        errx(77, "Couldn't create a GLES2 context\n");

    *out_dpy = dpy;
    *out_ctx = ctx;
}
#endif /* USE_EGL */

int
main(int argc, char **argv)
{
    bool pass = true;
#ifdef USE_EGL
    EGLDisplay *egl_dpy;
    EGLContext egl_ctx;
#endif
#ifdef USE_GLX
    Display *glx_dpy;
    GLXContext glx_ctx;
    Drawable glx_draw;
#endif

    /* Force epoxy to have loaded both EGL and GLX libs already -- we
     * can't assume anything about symbol resolution based on having
     * EGL or GLX loaded.
     */
    (void)glXGetCurrentContext();
    (void)eglGetCurrentContext();

#ifdef USE_GLX
    init_glx(&glx_dpy, &glx_ctx, &glx_draw);
    pass = make_glx_current_and_test(glx_dpy, glx_ctx, glx_draw) && pass;
#endif
#ifdef USE_EGL
    init_egl(&egl_dpy, &egl_ctx);
    pass = make_egl_current_and_test(egl_dpy, egl_ctx) && pass;
#endif

#if defined(USE_GLX) && defined(USE_EGL)
    pass = make_glx_current_and_test(glx_dpy, glx_ctx, glx_draw) && pass;
    pass = make_egl_current_and_test(egl_dpy, egl_ctx) && pass;
#endif

    return pass != true;
}
