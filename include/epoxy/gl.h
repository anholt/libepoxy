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

/** @file gl.h
 *
 * Provides an implementation of a GL dispatch layer using either
 * global function pointers or a hidden vtable.
 */

#ifndef EPOXY_GL_H
#define EPOXY_GL_H

#if    defined(__glplatform_h_)  || defined(__gl_h_)  || defined(__glext_h_)  \
    || defined(__gl2platform_h_) || defined(__gl2_h_) || defined(__gl2ext_h_) \
    || defined(__gl3platform_h_) || defined(__gl3_h_) || defined(__gl31_h_)

#error "epoxy/gl.h" must be included before (or in place of) the desktop OpenGL / OpenGL ES headers.
#endif

#define __glplatform_h_
#define __gl_h_
#define __glext_h_
#define __gl2platform_h
#define __gl2_h_ 1
#define __gl2ext_h_ 1
#define __gl3platform_h_
#define __gl3_h_ 1
#define __gl31_h_ 1

#include "epoxy/common.h"
#include "epoxy/khrplatform.h"
#ifdef _WIN32
#   include <Windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY KHRONOS_APIENTRY
#endif

#ifndef APIENTRY
#define APIENTRY GLAPIENTRY
#endif

#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPIENTRYP
#define GLAPIENTRYP GLAPIENTRY *
#endif

#define EPOXY_CALLSPEC KHRONOS_APIENTRY

#if (defined(__GNUC__) && __GNUC__ >= 4) || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
#   define GLAPI __attribute__((visibility("default")))
#else
#   define GLAPI extern
#endif

#include "epoxy/gl_generated.h"

EPOXY_IMPORTEXPORT bool epoxy_has_gl_extension(const char *extension);
EPOXY_IMPORTEXPORT bool epoxy_is_desktop_gl(void);
EPOXY_IMPORTEXPORT int epoxy_gl_version(void);
EPOXY_IMPORTEXPORT bool epoxy_current_context_is_egl(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* EPOXY_GL_H */
