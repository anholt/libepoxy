/* Copyright © 2013, Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* dladdr is a glibc extension */
#define _GNU_SOURCE
#include <dlfcn.h>

#include <assert.h>

#include "fips.h"

#include "dlwrap.h"

#include "glwrap.h"

void *libfips_handle;

typedef void *(*fips_dlopen_t)(const char *filename, int flag);
typedef void *(*fips_dlsym_t)(void *handle, const char *symbol);

static const char *wrapped_libs[] = {
    "libGL.so",
    "libEGL.so",
    "libGLESv2.so"
};

static void *orig_handles[ARRAY_SIZE(wrapped_libs)];

/* Match 'filename' against an internal list of libraries for which
 * libfips has wrappers.
 *
 * Returns true and sets *index_ret if a match is found.
 * Returns false if no match is found. */
static bool
find_wrapped_library_index(const char *filename, unsigned *index_ret)
{
    unsigned i;

    for (i = 0; i < ARRAY_SIZE(wrapped_libs); i++) {
        if (strncmp(wrapped_libs[i], filename,
                    strlen(wrapped_libs[i])) == 0)
            {
                *index_ret = i;
                return true;
            }
    }

    return false;
}

/* Perform a dlopen on the libfips library itself.
 *
 * Many places in fips need to lookup symbols within the libfips
 * library itself, (and not in any other library). This function
 * provides a reliable way to get a handle for performing such
 * lookups.
 *
 * The returned handle can be passed to dlwrap_real_dlsym for the
 * lookups. */
void *
dlwrap_dlopen_libfips(void)
{
    Dl_info info;

    /* We first find our own filename by looking up a function
     * known to exist only in libfips. This function itself
     * (dlwrap_dlopen_libfips) is a good one for that purpose. */
    if (dladdr(dlwrap_dlopen_libfips, &info) == 0) {
        fprintf(stderr, "Internal error: Failed to lookup filename of "
                "libfips library with dladdr\n");
        exit(1);
    }

    return dlwrap_real_dlopen(info.dli_fname, RTLD_NOW);
}

/* Many (most?) OpenGL programs dlopen libGL.so.1 rather than linking
 * against it directly, which means they would not be seeing our
 * wrapped GL symbols via LD_PRELOAD. So we catch the dlopen in a
 * wrapper here and redirect it to our library.
 */
void *
dlopen(const char *filename, int flag)
{
    void *ret;
    unsigned index;

    /* Before deciding whether to redirect this dlopen to our own
     * library, we call the real dlopen. This assures that any
     * expected side-effects from loading the intended library are
     * resolved. Below, we may still return a handle pointing to
     * our own library, and not what is opened here. */
    ret = dlwrap_real_dlopen(filename, flag);

    /* If filename is not a wrapped library, just return real dlopen */
    if (!find_wrapped_library_index(filename, &index))
        return ret;

    /* When the application dlopens any wrapped library starting
     * with 'libGL', (whether libGL.so.1 or libGLESv2.so.2), let's
     * continue to use that library handle for future lookups of
     * OpenGL functions. */
    if (STRNCMP_LITERAL(filename, "libGL") == 0)
        glwrap_set_gl_handle(ret);

    assert(index < ARRAY_SIZE(orig_handles));
    orig_handles[index] = ret;

    if (libfips_handle == NULL)
        libfips_handle = dlwrap_dlopen_libfips();

    /* Otherwise, we return our own handle so that we can intercept
     * future calls to dlsym. We encode the index in the return value
     * so that we can later map back to the originally requested
     * dlopen-handle if necessary. */
    return libfips_handle + index;
}

void *
dlwrap_real_dlopen(const char *filename, int flag)
{
    static fips_dlopen_t real_dlopen = NULL;

    if (!real_dlopen) {
        real_dlopen = (fips_dlopen_t) dlwrap_real_dlsym(RTLD_NEXT, "dlopen");
        if (!real_dlopen) {
            fprintf(stderr, "Error: Failed to find symbol for dlopen.\n");
            exit(1);
        }
    }

    return real_dlopen(filename, flag);
}

/* Since we redirect dlopens of libGL.so and libEGL.so to libfips we
 * need to ensure that dlysm succeeds for all functions that might be
 * defined in the real, underlying libGL library. But we're far too
 * lazy to implement wrappers for function that would simply
 * pass-through, so instead we also wrap dlysm and arrange for it to
 * pass things through with RTLD_next if libfips does not have the
 * function desired.  */
void *
dlsym(void *handle, const char *name)
{
    static void *symbol;
    unsigned index;

    /* All gl* and egl* symbols are preferentially looked up in libfips. */
    if (STRNCMP_LITERAL(name, "gl") == 0 || STRNCMP_LITERAL(name, "egl") == 0) {
        symbol = dlwrap_real_dlsym(libfips_handle, name);
        if (symbol)
            return symbol;
    }

    /* Failing that, anything specifically requested from the
     * libfips library should be redirected to a real GL
     * library. */

    /* We subtract the index back out of the handle (see the addition
     * of the index in our wrapper for dlopen above) to then use the
     * correct, original dlopen'ed handle for the library of
     * interest. */
    index = handle - libfips_handle;
    if (index < ARRAY_SIZE(orig_handles)) {
        return dlwrap_real_dlsym(orig_handles[index], name);
    }

    /* And anything else is some unrelated dlsym. Just pass it
     * through.  (This also covers the cases of lookups with
     * special handles such as RTLD_DEFAULT or RTLD_NEXT.)
     */
    return dlwrap_real_dlsym(handle, name);
}

void *
dlwrap_real_dlsym(void *handle, const char *name)
{
    static fips_dlsym_t real_dlsym = NULL;

    if (!real_dlsym) {
        /* FIXME: This brute-force, hard-coded searching for a versioned
         * symbol is really ugly. The only reason I'm doing this is because
         * I need some way to lookup the "dlsym" function in libdl, but
         * I can't use 'dlsym' to do it. So dlvsym works, but forces me
         * to guess what the right version is.
         *
         * Potential fixes here:
         *
         *   1. Use libelf to actually inspect libdl.so and
         *      find the right version, (finding the right
         *      libdl.so can be made easier with
         *      dl_iterate_phdr).
         *
         *   2. Use libelf to find the offset of the 'dlsym'
         *      symbol within libdl.so, (and then add this to
         *      the base address at which libdl.so is loaded
         *      as reported by dl_iterate_phdr).
         *
         * In the meantime, I'll just keep augmenting this
         * hard-coded version list as people report bugs. */
        const char *version[] = {
            "GLIBC_2.2.5",
            "GLIBC_2.0"
        };
        int num_versions = sizeof(version) / sizeof(version[0]);
        int i;
        for (i = 0; i < num_versions; i++) {
            real_dlsym = (fips_dlsym_t) dlvsym(RTLD_NEXT, "dlsym", version[i]);
            if (real_dlsym)
                break;
        }
        if (i == num_versions) {
            fprintf(stderr, "Internal error: Failed to find real dlsym\n");
            fprintf(stderr,
                    "This may be a simple matter of fips not knowing about the version of GLIBC that\n"
                    "your program is using. Current known versions are:\n\n\t");
            for (i = 0; i < num_versions; i++)
                fprintf(stderr, "%s ", version[i]);
            fprintf(stderr,
                    "\n\nYou can inspect your version by first finding libdl.so.2:\n"
                    "\n"
                    "\tldd <your-program> | grep libdl.so\n"
                    "\n"
                    "And then inspecting the version attached to the dlsym symbol:\n"
                    "\n"
                    "\treadelf -s /path/to/libdl.so.2 | grep dlsym\n"
                    "\n"
                    "And finally, adding the version to dlwrap.c:dlwrap_real_dlsym.\n");

            exit(1);
        }
    }

    return real_dlsym(handle, name);
}
