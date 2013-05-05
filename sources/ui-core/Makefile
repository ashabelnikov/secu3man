# SECU-3  - An open source, free engine control unit
# http://secu-3.org

# compiler
CC      = cl
# compiler flags
CFLAGS  = /nologo /W3 /GX /GR /MD /O2 /Ob2 -I.. /D_AFXDLL /D_AFXEXT /D_WINDLL /DWIN32 /D_WINDOWS /Ycstdafx.h
CFLAGS = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS = $(CFLAGS) -DDDX_HELPERS_EXPORTS -DFNT_HELPERS_EXPORTS -DXBROWSEFORFOLDER_EXPORTS
CFLAGS = $(CFLAGS) /D NDEBUG
CFLAGS = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# linker flags and directives
LFLAGS  = /nologo /machine:X86 /STACK:10000000 /release /dll /OUT:../../binaries/ui-core.dll /implib:../../libraries/ui-core.lib
# source files
SOURCES = StdAfx.cpp AnalogMeter.cpp AnalogMeterCtrl.cpp ddx_helpers.cpp fnt_helpers.cpp \
  DialogWithAccelerators.cpp EditEx.cpp HeaderCtrlEx.cpp HotKeysManager.cpp HotKeysToCmdRouter.cpp \
  ITabControllerEvent.cpp Label.cpp LEDIndicator.cpp MPCLogoPane.cpp MPCStatusBar.cpp OScopeCtrl.cpp \
  ProgressCtrlEx.cpp SpinButtonctrlEx.cpp TabController.cpp TabDialog.cpp ToolTipCtrlEx.cpp \
  UpdatableDialog.cpp WndSubclasser.cpp XBrowseForFolder.cpp uicore.cpp

# DLL building
all: uicore

uicore: uicore.cpp
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\ui-core.dll ( DEL /Q ..\..\binaries\ui-core.dll )
  @if exist ..\..\libraries\ui-core.lib ( DEL /Q ..\..\libraries\ui-core.lib )
  @if exist ..\..\libraries\ui-core.exp ( DEL /Q ..\..\libraries\ui-core.exp )
