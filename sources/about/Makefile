# compiler
CC      = cl
# resource compiler
RC      = rc
# compiler flags
CFLAGS  = /nologo /MD /GX /GR /O2 /Ob2 -I.. /D_AFXDLL /D_AFXEXT /D_WINDLL /DWIN32 /D_WINDOWS /Ycstdafx.h
CFLAGS  = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS  = $(CFLAGS) -DABOUT_EXPORTS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# linker flags and directives
LFLAGS  = /nologo /MACHINE:x86 /STACK:10000000 /release /DLL /OUT:../../binaries/about.dll /IMPLIB:../../libraries/about.lib about.res
# source files
SOURCES = stdafx.cpp secu-3about.cpp STCAbout.cpp about.cpp

# DLL building
all: about

about: about.cpp
  $(RC) about.rc
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\about.dll ( DEL /Q ..\..\binaries\about.dll )
  @if exist ..\..\libraries\about.lib ( DEL /Q ..\..\libraries\about.lib )
  @if exist ..\..\libraries\about.exp ( DEL /Q ..\..\libraries\about.exp )
