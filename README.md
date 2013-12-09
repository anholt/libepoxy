Epoxy is a library for handling OpenGL function pointer management for
you.

It hides the complexity of dlopen(), dlsym(), glXGetProcAddress(),
eglGetProcAddress(), etc. from the app developer, with very little
knowledge needed on their part.  Just read your GL specs and write
code using undecorated function names like glCompileShader().

Don't forget to check for your extensions or versions being present
before you use them, just like before!  We'll tell you what you forgot
to check for instead of just segfaulting, though.

Why does this library exist?
----------------------------

OpenGL on Linux (and other platforms) made some ABI decisions back in
the days when symbol versioning and dlsym() weren't as widely
available, that resulted in window-systems-specific APIs that looked
kind of like dlsym.  They allowed you to build an app that required
OpenGL 1.2, but could optionally use features of OpenGL 1.4 if the
implementation made those available through the glXGetProcAddress()
(or other similarly-named) mechanism.

The downside is that the fixed OpenGL 1.2 ABI means that application
developers have to GetProcAddress() out every modern GL entrypoint
they want to use and stash that function pointer somewhere.  Sometimes
this is done in a pretty way (like libGLEW), sometimes it is done in
an ad-hoc way (like most applications I've seen), but it's never done
as well as we think it could be done.

Additionally, the proliferation of OpenGL ABIs (desktop GL, GLESv1,
GLESv2) and window systems (GLX, AGL, WGL, all versus EGL) means that
an individual developer needs to know more and more about how to load
their classes symbols.

Switching your code to using epoxy
----------------------------------

It should be as easy as replacing:

    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>

with:

    #include <epoxy/gl.h>
    #include <epoxy/glx.h>

Additionally, some new helpers become available, so you don't have to
write them:

int epoxy_gl_version() returns the GL version:

12 for GL 1.2
20 for GL 2.0
44 for GL 4.4

bool epoxy_has_gl_extension() returns whether a GL extension is
available ("GL_ARB_texture_buffer_object", for example).

Note that this is not terribly fast, so keep it out of your hot paths,
ok?
