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

/** @file common.h
 *
 * Provides basic definitions for Epoxy. Included by all other Epoxy files.
 */

#ifndef EPOXY_COMMON_H
#define EPOXY_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined _WIN32 || defined __CYGWIN__
    #ifdef EPOXY_EXPORTS
        #ifdef __GNUC__
            #define EPOXY_IMPORTEXPORT __attribute__ ((dllexport))
        #else
            #define EPOXY_IMPORTEXPORT __declspec(dllexport)
        #endif
    #else
        #ifdef __GNUC__
            #define EPOXY_IMPORTEXPORT __attribute__ ((dllimport))
        #else
            #define EPOXY_IMPORTEXPORT __declspec(dllimport)
        #endif
    #endif
#else
    #if __GNUC__ >= 4
        #define EPOXY_IMPORTEXPORT __attribute__ ((visibility ("default")))
    #else
        #define EPOXY_IMPORTEXPORT
    #endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* EPOXY_COMMON_H */
