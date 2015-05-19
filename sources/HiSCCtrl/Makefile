# SECU-3  - An open source, free engine control unit
# http://secu-3.org

# compiler
CC      = cl
# compiler flags
CFLAGS  = /nologo /MD /GX /GR /O2 /Ob2 -I..\hiscctrl -D_AFXDLL -D_AFXEXT -D_WINDLL /DWIN32 /D_WINDOWS /Ycstdafx.h
CFLAGS  = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# linker flags and directives
LFLAGS  = /nologo /MACHINE:x86 /STACK:10000000 /release /DLL /OUT:../../binaries/hiscctrl.dll /IMPLIB:../../libraries/hiscctrl.lib hiscctrl.res
# source files
SOURCES = StdAfx.cpp sources/ChartAxis.cpp sources/ChartAxisLabel.cpp \
  sources/ChartCtrl.cpp sources/ChartGradient.cpp sources/ChartGrid.cpp sources/ChartLegend.cpp \
  sources/ChartLineSerie.cpp sources/ChartObject.cpp sources/ChartPointsSerie.cpp sources/ChartScrollBar.cpp \
  sources/ChartSerie.cpp sources/ChartTitle.cpp

# DLL building
all: hiscctrl

hiscctrl: hiscctrl.cpp
  $(RC) hiscctrl.rc
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\hiscctrl.dll ( DEL /Q ..\..\binaries\hiscctrl.dll )
  @if exist ..\..\libraries\hiscctrl.lib ( DEL /Q ..\..\libraries\hiscctrl.lib )
  @if exist ..\..\libraries\hiscctrl.exp ( DEL /Q ..\..\libraries\hiscctrl.exp )
