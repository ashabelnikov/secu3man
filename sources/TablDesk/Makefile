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
CFLAGS  = $(CFLAGS) -DDLLLF_EXPORTS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LIBS    = ui-core.lib
# linker flags and directives
LFLAGS  = /nologo /STACK:10000000 /MACHINE:x86 /release /DLL /OUT:../../binaries/tabldesk.dll /implib:../../libraries/tabldesk.lib /LIBPATH:../../libraries tabldesk.res
# source files
SOURCES = ButtonsPanel.cpp DLLLinkedFunctions.cpp DwellCalcDlg.cpp RPMGridEditDlg.cpp \
  StdAfx.cpp TablDesk.cpp TablesSetPanel.cpp GridModeEditorIgnDlg.cpp CESettingsDlg.cpp \
  GridModeEditorInjDlg.cpp GMEInjVEDlg.cpp GMEInjAFRDlg.cpp GMEInjITDlg.cpp GMEInjIRegDlg.cpp \
  GMEInjEnrDlg.cpp GMEInjOtherDlg.cpp AutoTuneController.cpp GMEInjPwm1Dlg.cpp GMEInjPwm2Dlg.cpp


# DLL building
all: tabldesk

tabldesk: tabldesk.cpp
  $(RC) tabldesk.rc
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(LIBS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\tabldesk.dll ( DEL /Q ..\..\binaries\tabldesk.dll )
  @if exist ..\..\libraries\tabldesk.lib ( DEL /Q ..\..\libraries\tabldesk.lib )
  @if exist ..\..\libraries\tabldesk.exp ( DEL /Q ..\..\libraries\tabldesk.exp )
