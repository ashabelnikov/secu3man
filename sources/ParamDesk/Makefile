# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
# http://secu-3.org

# compiler
CC      = cl
# resource compiler
RC      = rc
# compiler flags
CFLAGS  = /nologo /MD /GX /GR /O2 /Ob2 -I..\ParamDesk\Application -I.. -I../ParamDesk /D_AFXDLL /D_AFXEXT /D_WINDLL /DWIN32 /D_WINDOWS /Ycstdafx.h
CFLAGS  = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LIBS = ui-core.lib tabldesk.lib
# linker flags and directives
LFLAGS  = /nologo /STACK:10000000 /MACHINE:x86 /release /DLL /OUT:../../binaries/paramdesk.dll /IMPLIB:../../libraries/paramdesk.lib /LIBPATH:../../libraries paramdesk.res
# source files
SOURCES = Application/StdAfx.cpp Application/ParamDesk.cpp common/ParamPageEvents.cpp MAPCalc/MAPCalcController.cpp \
  MAPCalc/MAPCalcDlg.cpp Params/ADCCompenPageDlg.cpp Params/AnglesPageDlg.cpp Params/CarburPageDlg.cpp \
  Params/ChokePageDlg.cpp Params/CKPSPageDlg.cpp Params/FunSetPageDlg.cpp Params/IdlRegPageDlg.cpp \
  Params/KnockPageDlg.cpp Params/MiscPageDlg.cpp Params/ParamDeskDlg.cpp Params/StarterPageDlg.cpp \
  Params/TemperPageDlg.cpp Params/IORemappingDlg.cpp Params/SecurPageDlg.cpp Tables/TablesDeskDlg.cpp \
  Tables/TDContextMenuManager.cpp Params/InjectorPageDlg.cpp Params/LambdaPageDlg.cpp Params/AccelEnrPageDlg.cpp \
  common/ParamTabBaseDlg.cpp Params/UniOutPageDlg.cpp Params/GasdosePageDlg.cpp Tables/SepTablesDeskDlg.cpp

# DLL building
all: paramdesk

paramdesk: Application\paramdesk.cpp
  $(RC) /Foparamdesk.res Resources\paramdesk.rc
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(LIBS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\paramdesk.dll ( DEL /Q ..\..\binaries\paramdesk.dll )
  @if exist ..\..\libraries\paramdesk.lib ( DEL /Q ..\..\libraries\paramdesk.lib )
  @if exist ..\..\libraries\paramdesk.exp ( DEL /Q ..\..\libraries\paramdesk.exp )
