Epoxy is a library for handling OpenGL function pointer management for
you.

It hides the complexity of ```dlopen()```, ```dlsym()```,
```glXGetProcAddress()```, ```eglGetProcAddress()```, etc. from the
app developer, with very little knowledge needed on their part.  They
get to read GL specs and write code using undecorated function names
like ```glCompileShader()```.

Don't forget to check for your extensions or versions being present
before you use them, just like before!  We'll tell you what you forgot
to check for instead of just segfaulting, though.

Features
--------

* Automatically initializes as new OpenGL functions are used.
* Desktop OpenGL 4.4 core and compatibility context support.
* OpenGL ES 1/2/3 context support.
* Knows about function aliases so (e.g.) ```glBufferData()``` can be
  used with ```GL_ARB_vertex_buffer_object``` implementations, along
  with desktop OpenGL 1.5+ implementations.
* GLX, and WGL support.
* EGL support. EGL headers are included, so they're not necessary to build Epoxy
  with EGL support.
* Can be mixed with non-epoxy OpenGL usage.

Building (CMake)
-----------------

CMake is now the recommended way to build epoxy. It supports building both
shared and static libraries (by default only shared library is built). It also
supports building and running tests, both for the static and the shared library.

Building with CMake should be as simple as running:

    cd <my-build_dir>
    cmake -G <my-generator> <my-source-dir>

(run "cmake -h" see a list of possible generators). Then, to build the project,
depending on the type of generator you use, e.g. for Unix type "make", and for
MSVC open the solution in Visual studio and build the solution.

* NOTE: To build for 64 bit with MSVC add " Win64" to the generator name, e.g.
  "Visual studio 14 2015 Win64".

* To rebuild the generated headers from the specs, add
"-DEPOXY_REBUILD_FROM_SPECS=ON" to the "cmake" invocation.

* To build also static libraries, add
"-DEPOXY_BUILD_STATIC=ON" to the "cmake" invocation.

* To disable building shared libraries, add
"-DEPOXY_BUILD_SHARED=OFF" to the "cmake" invocation.

* To disable building tests, add
"-DEPOXY_BUILD_TESTS=OFF" to the "cmake" invocation.

* To link to the static Runtime Library with MSVC (rather than to the DLL), add
"-DEPOXY_MSVC_USE_RUNTIME_LIBRARY_DLL=OFF" to the "cmake" invocation.

Building (Autotools)
---------------------

On Unix you can also use autotools to build. This type of build only supports
building shared libraries. However it also supports building and running tests.
To build with autotools, write:

    ./autogen.sh
    make
    make check [optional]
    sudo make install

Dependencies for debian:

* libegl1-mesa-dev
* xutils-dev

Dependencies for OS X (macports):

* xorg-util-macros
* pkgconfig

The test suite has additional dependencies depending on the platform.
(X11, EGL, a running X Server).

Building (NMAKE)
-----------------

With MSVC you can also build directly with NMAKE. This type of build only
supports building shared libraries. However it also supports building
tests.

1. Check src\Makefile.vc to ensure that PYTHONDIR is pointing to your Python
   installation, either a 32-bit or a 64-bit (x64) installation of Python 2 or 3
   will do.
2. Copy "include\epoxy\config.h.guess" to "include\epoxy\config.h".
3. Open an MSVC Command prompt and run "nmake Makefile.vc CFG=release" or
   "nmake Makefile.vc CFG=debug" in src\ for a release or debug build.
4. Optionally, add src\ into your PATH and run the previous step in test\. Run
   the tests by running the built ".exe"-s.
5. Assuming you want to install in %INSTALL_DIR%, copy common.h, config.h,
   khrplatform.h, eglplatform.h, gl.h, gl_generated.h, wgl.h, wgl_generated.h,
   egl.h and egl_generated.h from include\epoxy\ to
   %INSTALL_DIR%\include\epoxy\, copy src\epoxy.lib to %INSTALL_DIR%\lib\ and
   copy epoxy-vs12.dll and epoxy-vs12.pdb (if you've built a debug build) from
   src\ to %INSTALL_DIR%\bin\. Create directories as needed.
6. To clean the project, repeat steps 2 and 3, adding " clean" to the commands.

Switching your Code to Use Epoxy
---------------------------------

* NOTE: If you use the static version of Epoxy, you must build your project with
  "EPOXY_STATIC_LIB" defined!

It should be as easy as replacing:

    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
    #include <EGL/egl.h>
    #include <EGL/eglext.h>
    #include <Windows.h> // for WGL

with:

    #include <epoxy/gl.h>
    #include <epoxy/glx.h>
    #include <epoxy/egl.h>
    #include <epoxy/wgl.h>

As long as epoxy's headers appear first, you should be ready to go.
Additionally, some new helpers become available, so you don't have to
write them:

```int epoxy_gl_version()``` returns the GL version:

* 12 for GL 1.2
* 20 for GL 2.0
* 44 for GL 4.4

```bool epoxy_has_gl_extension()``` returns whether a GL extension is
available (```GL_ARB_texture_buffer_object```, for example).

Note that this is not terribly fast, so keep it out of your hot paths,
ok?

Using OpenGL ES / EGL
----------------------

Building Epoxy with OpenGL ES / EGL support is now built-in. However, to
actually make use OpenGL ES and/or EGL on a computer, it's recommended (and in
some platforms necessary) to use an OpenGL ES / EGL emulator. I recommend using
[PowerVR SDK](http://community.imgtec.com/developers/powervr/graphics-sdk/),
which is available for Linux, OS X and Windows. Download it and run the
installer. In the installer, you don't have to check everything: Enough to check
"PowerVR Tools -> PVRVFrame" and "PowerVR SDK -> Native SDK". There's no need to
add anything from PowerVR SDK to the include directories to build or use Epoxy,
as it already includes all the necessary headers for using OpenGL ES / EGL.
There's also no need to link with anything from PowerVR SDK to build or use
Epoxy, as it loads the necessary libraries at run-time. However, when running
your app, if want to use EGL / OpenGL ES, you'll have to add the directory that
contains the right shared libraries ("GLES_CM", "GLESv2" and "EGL") to you
"PATH" environment variable. For instance, if you're on Windows, and used the
default locations when installing PowerVR SDK, then add
"C:\Imagination\PowerVR_Graphics\PowerVR_Tools\PVRVFrame\Library\Windows_x86_64"
to your "PATH" (for Windows 64 bit) or
"C:\Imagination\PowerVR_Graphics\PowerVR_Tools\PVRVFrame\Library\Windows_x86_32"
(for Windows 32 bit). For other platforms it would be something similar. Of
course, feel free to copy the shared libraries somewhere else.

Why not use GLEW?
--------------------

GLEW has several issues:

* Doesn't know about aliases of functions (There are 5 providers of
  glPointParameterfv, for example, and you don't want to have to
  choose which one to call when they're all the same).
* Doesn't support Desktop OpenGL 3.2+ core contexts.
* Doesn't support OpenGL ES.
* Doesn't support EGL.
* Has a hard-to-maintain parser of extension specification text
  instead of using the old .spec file or the new .xml.
* Has significant startup time overhead when ```glewInit()```
  autodetects the world.

The motivation for this project came out of previous use of libGLEW in
[piglit](http://piglit.freedesktop.org/).  Other GL dispatch code
generation projects had similar failures.  Ideally, piglit wants to be
able to build a single binary for a test that can run on whatever
context or window system it chooses, not based on link time choices.

We had to solve some of GLEW's problems for piglit and solving them
meant replacing every single piece of GLEW, so we built
piglit-dispatch from scratch.  And since we wanted to reuse it in
other GL-related projects, this is the result.

Windows issues
---------------

The automatic per-context symbol resolution for win32 requires that
epoxy knows when ```wglMakeCurrent()``` is called, because
wglGetProcAddress() return values depend on the context's device and
pixel format.  If ```wglMakeCurrent()``` is called from outside of
epoxy (in a way that might change the device or pixel format), then
epoxy needs to be notified of the change using the
```epoxy_handle_external_wglMakeCurrent()``` function.

The win32 wglMakeCurrent() variants are slower than they should be,
because they should be caching the resolved dispatch tables instead of
resetting an entire thread-local dispatch table every time.
