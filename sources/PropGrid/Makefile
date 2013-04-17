# SECU-3  - An open source, free engine control unit
# http://secu-3.org

# compiler
CC      = cl
# compiler flags
CFLAGS  = /nologo /MD /GX /GR /O2 /Ob2 -I.. /D_AFXDLL /D_AFXEXT /D_WINDLL /DWIN32 /D_WINDOWS /Ycstdafx.h
CFLAGS  = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = shlwapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LIBS = ui-core.lib
# linker flags and directives
LFLAGS  = /nologo /MACHINE:x86 /STACK:10000000 /release /DLL /OUT:../../binaries/propgrid.dll /IMPLIB:../../libraries/propgrid.lib /LIBPATH:../../libraries 
# source files
SOURCES = DynDialogEx.cpp DynDialogItemEx.cpp ListDynDialogEx.cpp PropGrid.cpp \
  PropertyGrid.cpp PropertyGridCombo.cpp PropertyGridDirectoryPicker.cpp PropertyGridInPlaceEdit.cpp \
  PropertyGridMonthCalCtrl.cpp PropertyGridSelector.cpp StdAfx.cpp

# DLL building
all: propgrid

propgrid: propgrid.cpp
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(LIBS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\propgrid.dll ( DEL /Q ..\..\binaries\propgrid.dll )
  @if exist ..\..\libraries\propgrid.lib ( DEL /Q ..\..\libraries\propgrid.lib )
  @if exist ..\..\libraries\propgrid.exp ( DEL /Q ..\..\libraries\propgrid.exp )
