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

#if !defined _WIN32
#error This file should only be used in Windows.
#endif
 
#include "dispatch_common.h"

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

        epoxy_first_context_current = false;

        /* FALLTHROUGH */

    case DLL_THREAD_ATTACH:
        data = LocalAlloc(LPTR, gl_tls_size);
        TlsSetValue(gl_tls_index, data);

        data = LocalAlloc(LPTR, wgl_tls_size);
        TlsSetValue(wgl_tls_index, data);

        break;

    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        data = TlsGetValue(gl_tls_index);
        LocalFree(data);

        data = TlsGetValue(wgl_tls_index);
        LocalFree(data);

        if (reason == DLL_PROCESS_DETACH) {
            TlsFree(gl_tls_index);
            TlsFree(wgl_tls_index);
        }
        break;
    }

    return TRUE;
}

#ifdef EPOXY_BUILDING_STATIC_LIB
#ifdef __GNUC__
	PIMAGE_TLS_CALLBACK dllmain_callback __attribute__((section(".CRT$XLB"))) = (PIMAGE_TLS_CALLBACK)DllMain;
#else
#	ifdef _WIN64
#		pragma comment(linker, "/INCLUDE:_tls_used")
#		pragma comment(linker, "/INCLUDE:dllmain_callback")
#		pragma const_seg(".CRT$XLB")
		extern const PIMAGE_TLS_CALLBACK dllmain_callback;
		const PIMAGE_TLS_CALLBACK dllmain_callback = DllMain;
#		pragma const_seg()
#	else
#		pragma comment(linker, "/INCLUDE:__tls_used")
#		pragma comment(linker, "/INCLUDE:_dllmain_callback")
#		pragma data_seg(".CRT$XLB")
		PIMAGE_TLS_CALLBACK dllmain_callback = DllMain;
#		pragma data_seg()
#	endif
#endif
#endif
