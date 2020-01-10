# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
# http://secu-3.org

# compiler
CC      = cl
# resource compiler
RC      = rc
# compiler flags
CFLAGS  = /nologo /MD /GX /GR /O2 /Ob2 -I.. /D_AFXDLL /D_AFXEXT /D_WINDLL /DWIN32 /D_WINDOWS /Ycstdafx.h
CFLAGS  = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LIBS    = io-core.lib ui-core.lib
# linker flags and directives
LFLAGS  = /nologo /MACHINE:x86 /STACK:10000000 /release /DLL /OUT:../../binaries/fwimpexp.dll /IMPLIB:../../libraries/fwimpexp.lib /LIBPATH:../../libraries fwimpexp.res
# source files
SOURCES =   StdAfx.cpp EEPROMImpExpController.cpp FwImpExp.cpp MapImpExpDlg.cpp MPSZFileDataIO.cpp \
  MPSZImpExpController.cpp S3FImpExpController.cpp S3FFileDataIO.cpp SECUImpExpController.cpp \
  FirmwareMasterDlg.cpp FirmwareMasterCntr.cpp 7z/7zAlloc.c 7z/CpuArch.c 7z/7zBuf.c 7z/7zCrc.c \
  7z/7zFile.c 7z/7zCrcOpt.c 7z/7zArcIn.c 7z/7zStream.c 7z/7zDec.c 7z/Bcj2.c 7z/Delta.c 7z/LzmaDec.c \
  7z/Lzma2Dec.c 7z/Ppmd7.c 7z/Bra.c 7z/Bra86.c 7z/BraIA64.c InjDrvFWMDlg.c InjDrvFWMCntr.c

# DLL building
all: fwimpexp

fwimpexp: fwimpexp.cpp
  $(RC) fwimpexp.rc
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(LIBS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\fwimpexp.dll ( DEL /Q ..\..\binaries\fwimpexp.dll )
  @if exist ..\..\libraries\fwimpexp.lib ( DEL /Q ..\..\libraries\fwimpexp.lib )
  @if exist ..\..\libraries\fwimpexp.exp ( DEL /Q ..\..\libraries\fwimpexp.exp )
