# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
# http://secu-3.org

# compiler
CC      = cl
# resource compiler
RC      = rc
# compiler flags
CFLAGS  = /nologo /W3 /GX /GR /O2 /Ob2 /DWIN32 /D_WINDOWS -I.. -I../secu3man -I../secu3man/Application -I../ParamDesk /MD -D_AFXDLL /Ycstdafx.h
CFLAGS  = $(CFLAGS) -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS
CFLAGS  = $(CFLAGS) /D NDEBUG
CFLAGS  = $(CFLAGS) -D_WIN32_WINDOWS=0x0501 -DWINVER=0x0501
# libraries
SYSLIBS = shlwapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LIBS    = about.lib hexutils.lib ui-core.lib io-core.lib fwimpexp.lib tabldesk.lib midesk.lib paramdesk.lib
# linker flags and directives
LFLAGS  = /nologo /subsystem:windows /MACHINE:x86 /release /OUT:../../binaries/secu3man.exe /LIBPATH:../../libraries secu3man.res
# source files
SOURCES = Application/StdAfx.cpp Application/secu3man.cpp Application/CommunicationManager.cpp \
  MainFrame/ChildView.cpp MainFrame/ChildViewManager.cpp MainFrame/MainFrame.cpp MainFrame/MainFrameController.cpp \
  MainFrame/MainFrameManager.cpp MainFrame/StatusBarManager.cpp Settings/AppSettingsController.cpp Settings/AppSettingsDlg.cpp \
  Settings/AppSettingsManager.cpp Settings/AppSettingsModel.cpp TabControllers/CheckEngineTabController.cpp TabControllers/DevDiagnostTabController.cpp \
  TabControllers/FirmwareTabController.cpp TabControllers/FWIORemappingController.cpp TabControllers/FWRPMGridEditController.cpp TabControllers/KnockChannelTabcontroller.cpp \
  Tabcontrollers/LogPlayerTabController.cpp TabControllers/ParamMonTabController.cpp TabControllers/PMInitDataCollector.cpp \
  TabControllers/PMMonitorController.cpp TabControllers/PMParamsController.cpp TabControllers/PMTablesController.cpp \
  TabControllers/TabcontrollersCommunicator.cpp TabDialogs/CheckEngineTabDlg.cpp TabDialogs/DevDiagnostTabDlg.cpp \
  TabDialogs/DiagnostContextMenuManager.cpp TabDialogs/FirmwareContextMenuManager.cpp TabDialogs/FirmwareTabDlg.cpp \
  TabDialogs/KnockChannelTabDlg.cpp TabDialogs/KnockContextMenuManager.cpp TabDialogs/KnockFrqCalcDlg.cpp \
  TabDialogs/LogPlayerTabDlg.cpp TabDialogs/LPControlPanelDlg.cpp TabDialogs/ParamMonTabDlg.cpp TabsManagement/MainTabController.cpp \
  TabControllers/FirmwareFileUtils.cpp TabDialogs/EEPROMTabDlg.cpp TabDialogs/EEPROMContextMenuManager.cpp TabControllers/EEPROMTabController.cpp \
  TabControllers/CEErrorIdStr.cpp TabControllers/ErrorMsg.cpp TabControllers/MapsWndScrPos.cpp TabDialogs/DevDiagInpsDlg.cpp TabDialogs/DevDiagOutsDlg.cpp \
  TabDialogs/InjDriverTabDlg.cpp TabControllers/InjDriverTabController.cpp TabControllers/InjDrvFileDataIO.cpp Settings/EditSettingsDlg.cpp


# EXE building
all: secu3man

secu3man: Application\secu3man.cpp
  $(RC) /Fosecu3man.res Resources\secu3man.rc
  $(CC) $(CFLAGS) $(SOURCES) /link$(LFLAGS) $(LIBS) $(SYSLIBS)

clean:
  @if exist *.obj ( DEL /Q *.obj )
  @if exist *.res ( DEL /Q *.res )
  @if exist *.pch ( DEL /Q *.pch )
  @if exist ..\..\binaries\secu3man.exe ( DEL /Q ..\..\binaries\secu3man.exe )
