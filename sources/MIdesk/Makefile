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
LIBS    = ui-core.lib
# linker flags and directives
LFLAGS  = /nologo /MACHINE:x86 /STACK:10000000 /release /DLL /OUT:../../binaries/midesk.dll /IMPLIB:../../libraries/midesk.lib /LIBPATH:../../libraries midesk.res 
# source files
SOURCES = StdAfx.cpp CEDeskDlg.cpp DVDeskDlg.cpp MIAirFlow.cpp MIDeskDlg.cpp MIDesk.cpp \
  MIDwellAngle.cpp MIPressure.cpp MITachometer.cpp \
  MITemperature.cpp MIThrottleGate.cpp MIVoltage.cpp MIVoltMeter.cpp RSDeskDlg.cpp LMDeskDlg.cpp \
  MIInjPW.cpp MIIAT.cpp MIEGOCorr.cpp MIVehicleSpeed.cpp MITPSDot.cpp MIMap2.cpp MIMapD.cpp \
  MITemp2.cpp MIFuelConsum.cpp MIKnockRetard.cpp MIKnockGraph.cpp MISensAFR.cpp MIChokePos.cpp \
  MISynLoad.cpp ContextMenuManager.cpp

# DLL building
all: midesk

midesk: midesk.cpp
  $(RC) midesk.rc
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(LIBS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\midesk.dll ( DEL /Q ..\..\binaries\midesk.dll )
  @if exist ..\..\libraries\midesk.lib ( DEL /Q ..\..\libraries\midesk.lib )
  @if exist ..\..\libraries\midesk.exp ( DEL /Q ..\..\libraries\midesk.exp )
