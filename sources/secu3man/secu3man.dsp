# Microsoft Developer Studio Project File - Name="secu3man" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=secu3man - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "secu3man.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "secu3man.mak" CFG="secu3man - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "secu3man - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "secu3man - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "secu3man - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /I "../" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib midesk.lib paramdesk.lib uicore.lib about.lib iocore.lib fwimpexp.lib hexutils.lib /nologo /subsystem:windows /machine:I386 /out:"../../binaries/secu3man.exe" /libpath:"../lib/" /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "secu3man - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib midesk.lib paramdesk.lib uicore.lib about.lib iocore.lib fwimpexp.lib hexutils.lib hiscctrl.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../binaries/secu3man.exe" /pdbtype:sept /libpath:"../output" /libpath:"../../libraries"

!ENDIF 

# Begin Target

# Name "secu3man - Win32 Release"
# Name "secu3man - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppSettingsController.cpp
# End Source File
# Begin Source File

SOURCE=.\AppSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AppSettingsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\AppSettingsModel.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckEngineTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckEngineTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildViewManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CommunicationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DLLLinkedFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\FirmwareContextMenuManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FirmwareTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\FirmwareTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\KnockChannelTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\KnockChannelTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrameManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MainTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\MainTabManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamMonTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamMonTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\secu3man.cpp
# End Source File
# Begin Source File

SOURCE=.\secu3man.rc
# End Source File
# Begin Source File

SOURCE=.\StatusBarManager.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AppSettingsController.h
# End Source File
# Begin Source File

SOURCE=.\AppSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\AppSettingsManager.h
# End Source File
# Begin Source File

SOURCE=.\AppSettingsModel.h
# End Source File
# Begin Source File

SOURCE=.\CheckEngineTabController.h
# End Source File
# Begin Source File

SOURCE=.\CheckEngineTabDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\ChildViewManager.h
# End Source File
# Begin Source File

SOURCE=.\CommunicationManager.h
# End Source File
# Begin Source File

SOURCE=.\DLLLinkedFunctions.h
# End Source File
# Begin Source File

SOURCE=.\FirmwareContextMenuManager.h
# End Source File
# Begin Source File

SOURCE=.\FirmwareTabController.h
# End Source File
# Begin Source File

SOURCE=.\FirmwareTabDlg.h
# End Source File
# Begin Source File

SOURCE=.\IAppSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\ISECU3man.h
# End Source File
# Begin Source File

SOURCE=.\ITabController.h
# End Source File
# Begin Source File

SOURCE=.\KnockChannelTabController.h
# End Source File
# Begin Source File

SOURCE=.\KnockChannelTabDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\MainFrameManager.h
# End Source File
# Begin Source File

SOURCE=.\MainTabController.h
# End Source File
# Begin Source File

SOURCE=.\MainTabManager.h
# End Source File
# Begin Source File

SOURCE=..\common\ObjectTimer.h
# End Source File
# Begin Source File

SOURCE=.\ParamMonTabController.h
# End Source File
# Begin Source File

SOURCE=.\ParamMonTabDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\secu3man.h
# End Source File
# Begin Source File

SOURCE=.\StatusBarManager.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\common\UnicodeSupport.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bootload.ico
# End Source File
# Begin Source File

SOURCE=.\res\ce_list_icons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\connected.ico
# End Source File
# Begin Source File

SOURCE=.\res\disconnected.ico
# End Source File
# Begin Source File

SOURCE=.\res\logwriting.ico
# End Source File
# Begin Source File

SOURCE=.\res\secu3man.ico
# End Source File
# Begin Source File

SOURCE=.\res\secu3man.rc2
# End Source File
# Begin Source File

SOURCE=.\res\tab_ctrl_bitmaps.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
