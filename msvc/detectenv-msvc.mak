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
CFLAGS_ADD = /MD /O2 /Zi
EXTRA_LDFLAGS = /opt:ref
!else
CFLAGS_ADD = /MDd /Od /Zi
EXTRA_LDFLAGS =
!endif

!if "$(PLAT)" == "x64"
LDFLAGS_ARCH = /machine:x64
!else
LDFLAGS_ARCH = /machine:x86
!endif

!if "$(C99_COMPAT_NEEDED)" == "1"
CFLAGS_C99_COMPAT = /I..\msvc /Dinline=__inline
!else
CFLAGS_C99_COMPAT = /Dinline=__inline
!endif
