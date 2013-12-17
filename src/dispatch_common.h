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

#include <stdbool.h>

#ifdef _WIN32
#define PLATFORM_HAS_EGL 0
#define PLATFORM_HAS_GLX 0
#define PLATFORM_HAS_WGL 1
#define EPOXYAPIENTRY __declspec(dllexport)
#elif defined(__APPLE__)
#define PLATFORM_HAS_EGL 0
#define PLATFORM_HAS_GLX 1
#define PLATFORM_HAS_WGL 0
#else
#define PLATFORM_HAS_EGL 1
#define PLATFORM_HAS_GLX 1
#define PLATFORM_HAS_WGL 0
#endif

#include "epoxy/gl.h"
#if PLATFORM_HAS_GLX
#include "epoxy/glx.h"
#endif
#if PLATFORM_HAS_EGL
#include "epoxy/egl.h"
#endif
#if PLATFORM_HAS_WGL
#include "epoxy/wgl.h"
#endif

#ifndef PUBLIC
#  ifdef _WIN32
#    define PUBLIC __declspec(dllexport)
#  elif (defined(__GNUC__) && __GNUC__ >= 4) || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
#    define PUBLIC __attribute__((visibility("default")))
#  else
#    define PUBLIC
#  endif
#endif

/* On win32, we're going to need to keep a per-thread dispatch table,
 * since the function pointers depend on the device and pixel format
 * of the current context.
 */
#if defined(_WIN32)
#define USING_DISPATCH_TABLE 1
#define UNWRAPPED_PROTO(x) x
#else
#define USING_DISPATCH_TABLE 0
#define UNWRAPPED_PROTO(x) (*x)
#endif

void *epoxy_egl_dlsym(const char *name);
void *epoxy_glx_dlsym(const char *name);
void *epoxy_gl_dlsym(const char *name);
void *epoxy_gles1_dlsym(const char *name);
void *epoxy_gles2_dlsym(const char *name);
void *epoxy_get_proc_address(const char *name);
void *epoxy_get_core_proc_address(const char *name, int core_version);

int epoxy_conservative_gl_version(void);
bool epoxy_conservative_has_gl_extension(const char *name);
int epoxy_conservative_glx_version(void);
bool epoxy_conservative_has_glx_extension(const char *name);
int epoxy_conservative_egl_version(void);
bool epoxy_conservative_has_egl_extension(const char *name);
bool epoxy_conservative_has_wgl_extension(const char *name);
void epoxy_print_failure_reasons(const char *name,
                                 const char **provider_names,
                                 const int *providers);

bool epoxy_extension_in_string(const char *extension_list, const char *ext);

extern void UNWRAPPED_PROTO(epoxy_glBegin_unwrapped)(GLenum primtype);
extern void UNWRAPPED_PROTO(epoxy_glEnd_unwrapped)(void);
