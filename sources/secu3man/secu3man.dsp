# Microsoft Developer Studio Project File - Name="secu3man" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=secu3man - Win32 UnicodeDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "secu3man.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "secu3man.mak" CFG="secu3man - Win32 UnicodeDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "secu3man - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "secu3man - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "secu3man - Win32 UnicodeDebug" (based on "Win32 (x86) Application")
!MESSAGE "secu3man - Win32 UnicodeRelease" (based on "Win32 (x86) Application")
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
# PROP Intermediate_Dir "../../intermediate/secu3man/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G4 /MD /W3 /GR /GX /Ot /Oi /I "../" /I "../secu3man/" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /I /../" " /c
# SUBTRACT CPP /Ox /Og /Os
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib midesk.lib paramdesk.lib uicore.lib about.lib iocore.lib fwimpexp.lib hexutils.lib tabldesk.lib /nologo /subsystem:windows /machine:I386 /out:"../../binaries/secu3man.exe" /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "secu3man - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "../../intermediate/secu3man/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G4 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../" /I "../secu3man/" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 hiscctrl.lib tabldesk.lib kernel32.lib midesk.lib paramdesk.lib uicore.lib about.lib iocore.lib fwimpexp.lib hexutils.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../binaries/secu3man.exe" /pdbtype:sept /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "secu3man - Win32 UnicodeDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "secu3man___Win32_UnicodeDebug"
# PROP BASE Intermediate_Dir "secu3man___Win32_UnicodeDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "../../intermediate/secu3man/UnicodeDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G4 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G4 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../" /I "../secu3man/" /D "_AFXDLL" /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib midesk.lib paramdesk.lib uicore.lib about.lib iocore.lib fwimpexp.lib hexutils.lib hiscctrl.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../binaries/secu3man.exe" /pdbtype:sept /libpath:"../output" /libpath:"../../libraries"
# ADD LINK32 hiscctrl.lib kernel32.lib midesk.lib paramdesk.lib uicore.lib about.lib iocore.lib fwimpexp.lib hexutils.lib tabldesk.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../../binaries/secu3man.exe" /pdbtype:sept /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "secu3man - Win32 UnicodeRelease"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "secu3man___Win32_UnicodeRelease"
# PROP BASE Intermediate_Dir "secu3man___Win32_UnicodeRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "../../intermediate/secu3man/UnicodeRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G4 /MD /W3 /GR /GX /Ot /Oi /I "../" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Ox /Og /Os
# ADD CPP /nologo /G4 /MD /W3 /GR /GX /Ot /Oi /I "../" /I "../secu3man/" /D "_MBCS" /D "_AFXDLL" /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Ox /Og /Os
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib midesk.lib paramdesk.lib uicore.lib about.lib iocore.lib fwimpexp.lib hexutils.lib /nologo /subsystem:windows /machine:I386 /out:"../../binaries/secu3man.exe" /libpath:"../lib/" /libpath:"../../libraries"
# ADD LINK32 kernel32.lib midesk.lib paramdesk.lib uicore.lib about.lib iocore.lib fwimpexp.lib hexutils.lib tabldesk.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"../../binaries/secu3man.exe" /libpath:"../../libraries"

!ENDIF 

# Begin Target

# Name "secu3man - Win32 Release"
# Name "secu3man - Win32 Debug"
# Name "secu3man - Win32 UnicodeDebug"
# Name "secu3man - Win32 UnicodeRelease"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "res"

# PROP Default_Filter ""
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
# End Group
# Begin Source File

SOURCE=.\Resources\res\bootload.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\res\connected.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\res\disconnected.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\res\logwriting.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Resources\res\secu3man.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\secu3man.rc
# End Source File
# End Group
# Begin Group "MainFrame"

# PROP Default_Filter "h;cpp;hpp"
# Begin Source File

SOURCE=.\MainFrame\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame\ChildViewManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame\ChildViewManager.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame\MainFrameController.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\MainFrame\MainFrameController.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame\MainFrameManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame\MainFrameManager.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame\StatusBarManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame\StatusBarManager.h
# End Source File
# End Group
# Begin Group "Settings"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Settings\AppSettingsController.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\AppSettingsController.h
# End Source File
# Begin Source File

SOURCE=.\Settings\AppSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\AppSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Settings\AppSettingsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\AppSettingsManager.h
# End Source File
# Begin Source File

SOURCE=.\Settings\AppSettingsModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\AppSettingsModel.h
# End Source File
# Begin Source File

SOURCE=.\Settings\IAppSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ISettingsData.h
# End Source File
# End Group
# Begin Group "TabControllers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TabControllers\CheckEngineTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\TabControllers\CheckEngineTabController.h
# End Source File
# Begin Source File

SOURCE=.\TabControllers\FirmwareTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\TabControllers\FirmwareTabController.h
# End Source File
# Begin Source File

SOURCE=.\TabControllers\KnockChannelTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\TabControllers\KnockChannelTabController.h
# End Source File
# Begin Source File

SOURCE=.\TabControllers\LogPlayerTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\TabControllers\LogPlayerTabController.h
# End Source File
# Begin Source File

SOURCE=.\TabControllers\ParamMonTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\TabControllers\ParamMonTabController.h
# End Source File
# Begin Source File

SOURCE=.\TabControllers\TabControllersCommunicator.cpp
# End Source File
# Begin Source File

SOURCE=.\TabControllers\TabControllersCommunicator.h
# End Source File
# End Group
# Begin Group "TabDialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TabDialogs\CheckEngineTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\CheckEngineTabDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\FirmwareContextMenuManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\FirmwareContextMenuManager.h
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\FirmwareTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\FirmwareTabDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\KnockChannelTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\KnockChannelTabDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\LogPlayerTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\LogPlayerTabDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\LPControlPanelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\LPControlPanelDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\ParamMonTabDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDialogs\ParamMonTabDlg.h
# End Source File
# End Group
# Begin Group "TabsManagement"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TabsManagement\ITabController.h
# End Source File
# Begin Source File

SOURCE=.\TabsManagement\MainTabController.cpp
# End Source File
# Begin Source File

SOURCE=.\TabsManagement\MainTabController.h
# End Source File
# Begin Source File

SOURCE=.\TabsManagement\MainTabManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TabsManagement\MainTabManager.h
# End Source File
# End Group
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Application\CommunicationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Application\CommunicationManager.h
# End Source File
# Begin Source File

SOURCE=.\Application\ISECU3man.h
# End Source File
# Begin Source File

SOURCE=.\Application\secu3man.cpp
# End Source File
# Begin Source File

SOURCE=.\Application\secu3man.h
# End Source File
# Begin Source File

SOURCE=.\Application\StdAfx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\Application\StdAfx.h
# End Source File
# End Group
# End Target
# End Project
