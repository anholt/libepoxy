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
#include <ctype.h>
#include <stdio.h>
#include <pthread.h>
#include <err.h>
#include "epoxy/gl.h"
#include "epoxy/glx.h"
#include "epoxy/egl.h"
#include "dispatch_common.h"

struct api {
    /**
     * Locking for making sure we don't double-dlopen().
     */
    pthread_mutex_t mutex;

    /** dlopen() return value for libGL.so.1. */
    void *glx_handle;

    /** dlopen() return value for libEGL.so.1 */
    void *egl_handle;

    /** dlopen() return value for libGLESv1_CM.so.1 */
    void *gles1_handle;

    /** dlopen() return value for libGLESv2.so.2 */
    void *gles2_handle;

    /**
     * This value gets incremented when any thread is in
     * glBegin()/glEnd() called through epoxy.
     *
     * We're not guaranteed to be called through our wrapper, so the
     * conservative paths also try to handle the failure cases they'll
     * see if begin_count didn't reflect reality.  It's also a bit of
     * a bug that the conservative paths might return success because
     * some other thread was in epoxy glBegin/glEnd while our thread
     * is trying to resolve, but given that it's basically just for
     * informative error messages, we shouldn't need to care.
     */
    int begin_count;
};

static struct api api = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
};

static void
get_dlopen_handle(void **handle, const char *lib_name, bool exit_on_fail)
{
    if (*handle)
        return;

    pthread_mutex_lock(&api.mutex);
    if (!*handle) {
        *handle = dlopen(lib_name, RTLD_LAZY | RTLD_LOCAL);
        if (!*handle && exit_on_fail) {
            fprintf(stderr, "Couldn't open %s: %s", lib_name, dlerror());
            exit(1);
        }
    }
    pthread_mutex_unlock(&api.mutex);
}

static void *
do_dlsym(void **handle, const char *lib_name, const char *name,
         bool exit_on_fail)
{
    void *result;

    get_dlopen_handle(handle, lib_name, exit_on_fail);

    result = dlsym(*handle, name);
    if (!result)
        errx(1, "%s() not found in %s", name, lib_name);

    return result;
}

PUBLIC bool
epoxy_is_desktop_gl(void)
{
    const char *es_prefix = "OpenGL ES ";
    const char *version;

    if (api.begin_count)
        return true;

    version = (const char *)glGetString(GL_VERSION);

    /* If we didn't get a version back, there are only two things that
     * could have happened: either malloc failure (which basically
     * doesn't exist), or we were called within a glBegin()/glEnd().
     * Assume the second, which only exists for desktop GL.
     */
    if (!version)
        return true;

    return strncmp(es_prefix, version, strlen(es_prefix));
}

static int
epoxy_internal_gl_version(int error_version)
{
    const char *version = (const char *)glGetString(GL_VERSION);
    GLint major, minor;
    int scanf_count;

    if (!version)
        return error_version;

    /* skip to version number */
    while (!isdigit(*version) && *version != '\0')
        version++;

    /* Interpret version number */
    scanf_count = sscanf(version, "%i.%i", &major, &minor);
    if (scanf_count != 2) {
        fprintf(stderr, "Unable to interpret GL_VERSION string: %s\n",
                version);
        exit(1);
    }
    return 10 * major + minor;
}

PUBLIC int
epoxy_gl_version(void)
{
    return epoxy_internal_gl_version(0);
}

PUBLIC int
epoxy_conservative_gl_version(void)
{
    if (api.begin_count)
        return 100;

    return epoxy_internal_gl_version(100);
}

/**
 * If we can determine the GLX version from the current context, then
 * return that, otherwise return a version that will just send us on
 * to dlsym() or get_proc_address().
 */
int
epoxy_conservative_glx_version(void)
{
    Display *dpy = glXGetCurrentDisplay();
    GLXContext ctx = glXGetCurrentContext();
    int screen;

    if (!dpy || !ctx)
        return 14;

    glXQueryContext(dpy, ctx, GLX_SCREEN, &screen);

    return epoxy_glx_version(dpy, screen);
}

PUBLIC int
epoxy_glx_version(Display *dpy, int screen)
{
    int server_major, server_minor;
    int client_major, client_minor;
    int server, client;
    const char *version_string;
    int ret;

    version_string = glXQueryServerString(dpy, screen, GLX_VERSION);
    ret = sscanf(version_string, "%d.%d", &server_major, &server_minor);
    assert(ret == 2);
    server = server_major * 10 + server_minor;

    version_string = glXGetClientString(dpy, GLX_VERSION);
    ret = sscanf(version_string, "%d.%d", &client_major, &client_minor);
    assert(ret == 2);
    client = client_major * 10 + client_minor;

    if (client < server)
        return client;
    else
        return server;
}

PUBLIC int
epoxy_conservative_egl_version(void)
{
    EGLDisplay *dpy = eglGetCurrentDisplay();

    if (!dpy)
        return 14;

    return epoxy_egl_version(dpy);
}

PUBLIC int
epoxy_egl_version(EGLDisplay *dpy)
{
    int major, minor;
    const char *version_string;
    int ret;

    version_string = eglQueryString(dpy, EGL_VERSION);
    ret = sscanf(version_string, "%d.%d", &major, &minor);
    assert(ret == 2);
    return major * 10 + minor;
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

static bool
epoxy_internal_has_gl_extension(const char *ext, bool invalid_op_mode)
{
    if (epoxy_gl_version() < 30) {
        const char *exts = (const char *)glGetString(GL_EXTENSIONS);
        if (!exts)
            return invalid_op_mode;
        return epoxy_extension_in_string(exts, ext);
    } else {
        int num_extensions;

        glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
        if (num_extensions == 0)
            return invalid_op_mode;

        for (int i = 0; i < num_extensions; i++) {
            const char *gl_ext = (const char *)glGetStringi(GL_EXTENSIONS, i);
            if (strcmp(ext, gl_ext) == 0)
                return true;
        }

        return false;
    }
}

/**
 * Returns true if the given GL extension is supported in the current context.
 *
 * Note that this function can't be called from within glBegin()/glEnd().
 *
 * \sa epoxy_has_egl_extension()
 * \sa epoxy_has_glx_extension()
 */
PUBLIC bool
epoxy_has_gl_extension(const char *ext)
{
    return epoxy_internal_has_gl_extension(ext, false);
}

bool
epoxy_conservative_has_gl_extension(const char *ext)
{
    if (api.begin_count)
        return true;

    return epoxy_internal_has_gl_extension(ext, true);
}

bool
epoxy_conservative_has_egl_extension(const char *ext)
{
    EGLDisplay *dpy = eglGetCurrentDisplay();

    if (!dpy)
        return true;

    return epoxy_has_egl_extension(dpy, ext);
}

PUBLIC bool
epoxy_has_egl_extension(EGLDisplay *dpy, const char *ext)
{
    return epoxy_extension_in_string(eglQueryString(dpy, EGL_EXTENSIONS), ext);
}

/**
 * If we can determine the GLX extension support from the current
 * context, then return that, otherwise give the answer that will just
 * send us on to get_proc_address().
 */
bool
epoxy_conservative_has_glx_extension(const char *ext)
{
    Display *dpy = glXGetCurrentDisplay();
    GLXContext ctx = glXGetCurrentContext();
    int screen;

    if (!dpy || !ctx)
        return true;

    glXQueryContext(dpy, ctx, GLX_SCREEN, &screen);

    return epoxy_has_glx_extension(dpy, screen, ext);
}

PUBLIC bool
epoxy_has_glx_extension(Display *dpy, int screen, const char *ext)
 {
    /* No, you can't just use glXGetClientString or
     * glXGetServerString() here.  Those each tell you about one half
     * of what's needed for an extension to be supported, and
     * glXQueryExtensionsString() is what gives you the intersection
     * of the two.
     */
    return epoxy_extension_in_string(glXQueryExtensionsString(dpy, screen), ext);
}

void *
epoxy_egl_dlsym(const char *name)
{
    return do_dlsym(&api.egl_handle, "libEGL.so.1", name, true);
}

void *
epoxy_glx_dlsym(const char *name)
{
    return do_dlsym(&api.glx_handle, "libGL.so.1", name, true);
}

void *
epoxy_gl_dlsym(const char *name)
{
    /* There's no library for desktop GL support independent of GLX. */
    return epoxy_glx_dlsym(name);
}

void *
epoxy_gles1_dlsym(const char *name)
{
    return do_dlsym(&api.gles1_handle, "libGLESv1_CM.so.1", name, true);
}

void *
epoxy_gles2_dlsym(const char *name)
{
    return do_dlsym(&api.gles1_handle, "libGLESv2.so.2", name, true);
}

void *
epoxy_get_proc_address(const char *name)
{
    if (api.egl_handle) {
        return eglGetProcAddress(name);
    } else if (api.glx_handle) {
        return glXGetProcAddressARB((const GLubyte *)name);
    } else {
        /* If the application hasn't explicitly called some of our GLX
         * or EGL code but has presumably set up a context on its own,
         * then we need to figure out how to getprocaddress anyway.
         *
         * If there's a public GetProcAddress loaded in the
         * application's namespace, then use that.
         */
        PFNGLXGETPROCADDRESSARBPROC glx_gpa;
        PFNEGLGETPROCADDRESSPROC egl_gpa;

        egl_gpa = dlsym(NULL, "eglGetProcAddress");
        if (egl_gpa)
            return egl_gpa(name);

        glx_gpa = dlsym(NULL, "glXGetProcAddressARB");
        if (glx_gpa)
            return glx_gpa((const GLubyte *)name);

        /* OK, couldn't find anything in the app's address space.
         * Presumably they dlopened with RTLD_LOCAL, which hides it
         * from us.  Just go dlopen()ing likely libraries and try them.
         */
        egl_gpa = do_dlsym(&api.egl_handle, "libEGL.so.1", "eglGetProcAddress",
                           false);
        if (egl_gpa)
            return egl_gpa(name);

        return do_dlsym(&api.glx_handle, "libGL.so.1", "glXGetProcAddressARB",
                        false);
        if (glx_gpa)
            return glx_gpa((const GLubyte *)name);

        errx(1, "Couldn't find GLX or EGL libraries.\n");
    }
}

void
epoxy_print_failure_reasons(const char *name,
                            const char **provider_names,
                            const int *providers)
{
    int i;

    fprintf(stderr, "No provider of %s found.  "
            "Requires one of:\n", name);

    for (i = 0; providers[i] != 0; i++)
        fputs(stderr, provider_names[providers[i]]);
}

PUBLIC void
epoxy_glBegin(GLenum primtype)
{
    pthread_mutex_lock(&api.mutex);
    api.begin_count++;
    pthread_mutex_unlock(&api.mutex);

    epoxy_glBegin_unwrapped(primtype);
}

PUBLIC void
epoxy_glEnd(void)
{
    epoxy_glEnd_unwrapped();

    pthread_mutex_lock(&api.mutex);
    api.begin_count--;
    pthread_mutex_unlock(&api.mutex);
}
