/*
 * Copyright © 2013 Intel Corporation
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
 * @file dispatch_common.c
 *
 * Implements common code shared by the generated GL/EGL/GLX dispatch code.
 *
 * A collection of some important specs on getting GL function pointers.
 *
 * From the linux GL ABI (http://www.opengl.org/registry/ABI/):
 *
 *     "3.4. The libraries must export all OpenGL 1.2, GLU 1.3, GLX 1.3, and
 *           ARB_multitexture entry points statically.
 *
 *      3.5. Because non-ARB extensions vary so widely and are constantly
 *           increasing in number, it's infeasible to require that they all be
 *           supported, and extensions can always be added to hardware drivers
 *           after the base link libraries are released. These drivers are
 *           dynamically loaded by libGL, so extensions not in the base
 *           library must also be obtained dynamically.
 *
 *      3.6. To perform the dynamic query, libGL also must export an entry
 *           point called
 *
 *           void (*glXGetProcAddressARB(const GLubyte *))(); 
 *
 *      The full specification of this function is available separately. It
 *      takes the string name of a GL or GLX entry point and returns a pointer
 *      to a function implementing that entry point. It is functionally
 *      identical to the wglGetProcAddress query defined by the Windows OpenGL
 *      library, except that the function pointers returned are context
 *      independent, unlike the WGL query."
 *
 * From the EGL 1.4 spec:
 *
 *    "Client API function pointers returned by eglGetProcAddress are
 *     independent of the display and the currently bound client API context,
 *     and may be used by any client API context which supports the extension.
 *
 *     eglGetProcAddress may be queried for all of the following functions:
 *
 *     • All EGL and client API extension functions supported by the
 *       implementation (whether those extensions are supported by the current
 *       client API context or not). This includes any mandatory OpenGL ES
 *       extensions.
 *
 *     eglGetProcAddress may not be queried for core (non-extension) functions
 *     in EGL or client APIs 20 .
 *
 *     For functions that are queryable with eglGetProcAddress,
 *     implementations may choose to also export those functions statically
 *     from the object libraries im- plementing those functions. However,
 *     portable clients cannot rely on this behavior.
 *
 * From the GLX 1.4 spec:
 *
 *     "glXGetProcAddress may be queried for all of the following functions:
 *
 *      • All GL and GLX extension functions supported by the implementation
 *        (whether those extensions are supported by the current context or
 *        not).
 *
 *      • All core (non-extension) functions in GL and GLX from version 1.0 up
 *        to and including the versions of those specifications supported by
 *        the implementation, as determined by glGetString(GL VERSION) and
 *        glXQueryVersion queries."
 */

#include <assert.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include "epoxy/gl.h"
#include "epoxy/glx.h"
#include "dispatch_common.h"

/* XXX: Make this thread local */
struct api local_api;

struct api *api = &local_api;

bool
epoxy_is_desktop_gl(void)
{
    const char *es_prefix = "OpenGL ES ";
    const char *version = (const char *)glGetString(GL_VERSION);

    return strncmp(es_prefix, version, strlen(es_prefix));
}

PUBLIC int
epoxy_gl_version(void)
{
    GLint major, minor;

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MAJOR_VERSION, &minor);

    return major * 10 + minor;
}

bool
epoxy_is_glx(void)
{
    return true; /* XXX */
}

int
epoxy_glx_version(void)
{
    return 14; /* XXX */
}

static bool
epoxy_extension_in_string(const char *extension_list, const char *ext)
{
    const char *ptr = extension_list;
    int len = strlen(ext);

    /* Make sure that don't just find an extension with our name as a prefix. */
    do {
        ptr = strstr(ptr, ext);
    } while (ptr && (ptr[len] != ' ' && ptr[len] != 0));

    return ptr != NULL;
}

PUBLIC bool
epoxy_has_gl_extension(const char *ext)
{
    return epoxy_extension_in_string((const char *)glGetString(GL_EXTENSIONS),
                                     ext);
}

#if 0
PUBLIC bool
epoxy_has_egl_extension(const char *ext)
{
    return epoxy_extension_in_string(eglQueryString(EGL_EXTENSIONS), ext);
}
#endif

PUBLIC bool
epoxy_has_glx_extension(const char *ext)
{
    Display *dpy = glXGetCurrentDisplay();
    int screen = 0;

    if (!dpy) {
        fprintf(stderr, "waffle needs a display!"); /* XXX */
        return false;
    }

    /* No, you can't just use glXGetClientString or glXGetServerString() here.
     * Those each tell you about one half of what's needed for an extension to
     * be supported, and glXQueryExtensionsString().
     */
    return epoxy_extension_in_string(glXQueryExtensionsString(dpy, screen), ext);
}

void *
epoxy_dlsym(const char *name)
{
    assert(api->gl_handle);
    return dlsym(api->gl_handle, name);
}

void *
epoxy_get_proc_address(const char *name)
{
    return glXGetProcAddress((const GLubyte *)name);
}

void
epoxy_glx_autoinit(void)
{
    if (api->gl_handle)
        return;

    api->gl_handle = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!api->gl_handle) {
        fprintf(stderr, "Couldn't open libGL.so.1: %s", dlerror());
        exit(1);
    }

    api->winsys_handle = api->gl_handle;
}

void
epoxy_platform_autoinit(void)
{
    epoxy_glx_autoinit();
}

