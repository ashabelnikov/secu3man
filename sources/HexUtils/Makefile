# SECU-3  - An open source, free engine control unit
# Created by Alexey A. Shabelnikov, UA, Kiev. http://secu-3.org

# compiler
CC      = cl
# compiler flags
CFLAGS  = /nologo /MD /GX /GR /O2 /Ob2 -I.. /D_AFXDLL /D_AFXEXT /D_WINDLL /DWIN32 /D_WINDOWS /Ycstdafx.h
CFLAGS  = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS  = $(CFLAGS) -DHEXUTILS_EXPORTS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# linker flags and directives
LFLAGS  = /nologo /MACHINE:x86 /STACK:10000000 /release /DLL /OUT:../../binaries/hexutils.dll /IMPLIB:../../libraries/hexutils.lib hexutils.res
# source files
SOURCES = stdafx.cpp hexutils.cpp readhex.cpp

# DLL building
all: hexutils

hexutils: hexutils.cpp
  $(RC) hexutils.rc
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\hexutils.dll ( DEL /Q ..\..\binaries\hexutils.dll )
  @if exist ..\..\libraries\hexutils.lib ( DEL /Q ..\..\libraries\hexutils.lib )
  @if exist ..\..\libraries\hexutils.exp ( DEL /Q ..\..\libraries\hexutils.exp )
