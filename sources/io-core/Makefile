# compiler
CC      = cl
# compiler flags
CFLAGS  = /nologo /MD /GX /GR /O2 /Ob2 -I.. /D_AFXDLL /D_AFXEXT /D_WINDLL /DWIN32 /D_WINDOWS /Ycstdafx.h
CFLAGS  = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS  = $(CFLAGS) -DIOCORE_EXPORTS -DCRC_EXPORTS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# linker flags and directives
LFLAGS  = /nologo /MACHINE:x86 /STACK:10000000 /release /DLL /OUT:../../binaries/io-core.dll /IMPLIB:../../libraries/io-core.lib
# source files
SOURCES = StdAfx.cpp BootLoader.cpp BootLoaderAdapter.cpp ccomport.cpp ControlApp.cpp \
  ControlAppAdapter.cpp CRC16.cpp EEPROMDataMediator.cpp FirmwareDataMediator.cpp iocore.cpp \
  LogReader.cpp LogWriter.cpp NumericConv.cpp PlatformParamHolder.cpp

# DLL building
all: iocore

iocore: iocore.cpp
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\io-core.dll ( DEL /Q ..\..\binaries\io-core.dll )
  @if exist ..\..\libraries\io-core.lib ( DEL /Q ..\..\libraries\io-core.lib )
  @if exist ..\..\libraries\io-core.exp ( DEL /Q ..\..\libraries\io-core.exp )
