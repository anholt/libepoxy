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
#include "epoxy/gl.h"
#include "epoxy/glx.h"

#ifndef PUBLIC
#  if (defined(__GNUC__) && __GNUC__ >= 4) || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
#    define PUBLIC __attribute__((visibility("default")))
#  else
#    define PUBLIC
#  endif
#endif

struct api {
	bool is_glx;
	bool is_gl;
	bool is_gles1;
	bool is_gles2; /**< Also GLES3 */

	/** dlopen() return value for libGL.so.1, libGLESv2.so.1, etc. */
	void *gl_handle;

	/** dlopen() return value for libGL.so.1 or libEGL.so.1 */
	void *winsys_handle;
};

bool epoxy_is_glx(void);

void *epoxy_get_proc_address(const char *name);
int epoxy_conservative_glx_version(void);
bool epoxy_conservative_has_glx_extension(const char *name);
void *epoxy_dlsym(const char *name);
void epoxy_glx_autoinit(void);
void epoxy_platform_autoinit(void);
void epoxy_print_failure_reasons(const char *name,
                                 const char **provider_names,
                                 const int *providers);
