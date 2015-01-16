# Copyright © 2015 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

# Check to see we are configured to build with MSVC (MSDEVDIR, MSVCDIR or
# VCINSTALLDIR) or with the MS Platform SDK (MSSDK or WindowsSDKDir)
!if !defined(VCINSTALLDIR) && !defined(WINDOWSSDKDIR)
MSG = ^
This Makefile is only for Visual Studio 2008 and later.^
You need to ensure that the Visual Studio Environment is properly set up^
before running this Makefile.
!error $(MSG)
!endif

ERRNUL  = 2>NUL
_HASH=^#

!if ![echo VCVERSION=_MSC_VER > vercl.x] \
    && ![echo $(_HASH)if defined(_M_IX86) >> vercl.x] \
    && ![echo PLAT=Win32 >> vercl.x] \
    && ![echo $(_HASH)elif defined(_M_AMD64) >> vercl.x] \
    && ![echo PLAT=x64 >> vercl.x] \
    && ![echo $(_HASH)endif >> vercl.x] \
    && ![cl -nologo -TC -P vercl.x $(ERRNUL)]
!include vercl.i
!if ![echo VCVER= ^\> vercl.vc] \
    && ![set /a $(VCVERSION) / 100 - 6 >> vercl.vc]
!include vercl.vc
!endif
!endif
!if ![del $(ERRNUL) /q/f vercl.x vercl.i vercl.vc]
!endif

!if $(VCVERSION) > 1499 && $(VCVERSION) < 1600
VSVER = 9
C99_COMPAT_NEEDED = 1
!elseif $(VCVERSION) > 1599 && $(VCVERSION) < 1700
VSVER = 10
C99_COMPAT_NEEDED = 1
!elseif $(VCVERSION) > 1699 && $(VCVERSION) < 1800
VSVER = 11
C99_COMPAT_NEEDED = 1
!elseif $(VCVERSION) > 1799 && $(VCVERSION) < 1900
VSVER = 12
C99_COMPAT_NEEDED = 0
!else
VSVER = 0
!endif

!if "$(VSVER)" == "0"
MSG = ^
This NMake Makefile set supports Visual Studio^
9 (2008) through 12 (2013).  Your Visual Studio^
version is not supported.
!error $(MSG)
!endif

VALID_CFGSET = FALSE
!if "$(CFG)" == "release" || "$(CFG)" == "debug"
VALID_CFGSET = TRUE
!endif

!if "$(VALID_CFGSET)" == "FALSE"
MSG = ^
You need to specify CFG=release or CFG=debug.
!error $(MSG)
!endif

!if "$(CFG)" == "release"
CFLAGS_ADD = /MD /O2 /Zi /I..\include /I..\..\vs$(VSVER)\$(PLAT)\include
EXTRA_LDFLAGS = /opt:ref
!else
CFLAGS_ADD = /MDd /Od /Zi /I..\include /I..\..\vs$(VSVER)\$(PLAT)\include
EXTRA_LDFLAGS =
!endif

!if "$(PLAT)" == "x64"
LDFLAGS_ARCH = /machine:x64
!else
LDFLAGS_ARCH = /machine:x86
!endif

CFLAGS_C99_COMPAT = /Dinline=__inline
