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

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "dispatch_common.h"

/**
 * If we can determine the WGL extension support from the current
 * context, then return that, otherwise give the answer that will just
 * send us on to get_proc_address().
 */
bool
epoxy_conservative_has_wgl_extension(const char *ext)
{
    HDC hdc = wglGetCurrentDC();

    if (!hdc)
        return true;

    return epoxy_has_wgl_extension(hdc, ext);
}

PUBLIC bool
epoxy_has_wgl_extension(HDC hdc, const char *ext)
 {
     PFNWGLGETEXTENSIONSSTRINGARBPROC getext;

     getext = (void *)wglGetProcAddress("wglGetExtensionsStringARB");
     if (!getext) {
         fprintf(stderr,
                 "Implementation unexpectedly missing "
                 "WGL_ARB_extensions_string.  Probably a libepoxy bug.\n");
         return false;
     }

    return epoxy_extension_in_string(getext(hdc), ext);
}

static void
reset_dispatch_table(void)
{
    gl_init_dispatch_table();
    wgl_init_dispatch_table();
}

/**
 * This global symbol is apparently looked up by Windows when loading
 * a DLL, but it doesn't declare the prototype.
 */
BOOL WINAPI
DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved);

BOOL WINAPI
DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
    void *data;

    switch (reason) {
    case DLL_PROCESS_ATTACH:
        gl_tls_index = TlsAlloc();
        if (gl_tls_index == TLS_OUT_OF_INDEXES)
            return FALSE;
        wgl_tls_index = TlsAlloc();
        if (wgl_tls_index == TLS_OUT_OF_INDEXES)
            return FALSE;

        /* FALLTHROUGH */

    case DLL_THREAD_ATTACH:
        data = LocalAlloc(LPTR, gl_tls_size);
        TlsSetValue(gl_tls_index, data);

        data = LocalAlloc(LPTR, wgl_tls_size);
        TlsSetValue(wgl_tls_index, data);

        reset_dispatch_table();
        break;

    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        data = TlsGetValue(gl_tls_index);
        LocalFree(data);

        data = TlsGetValue(wgl_tls_index);
        LocalFree(data);
        break;

        if (reason == DLL_PROCESS_DETACH) {
            TlsFree(gl_tls_index);
            TlsFree(wgl_tls_index);
        }
        break;
    }

    return TRUE;
}
