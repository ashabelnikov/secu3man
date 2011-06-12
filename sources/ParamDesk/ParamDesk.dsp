# Microsoft Developer Studio Project File - Name="ParamDesk" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ParamDesk - Win32 UnicodeDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ParamDesk.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ParamDesk.mak" CFG="ParamDesk - Win32 UnicodeDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ParamDesk - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ParamDesk - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ParamDesk - Win32 UnicodeDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ParamDesk - Win32 UnicodeRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ParamDesk - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "../../intermediate/ParamDesk/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G4 /MD /W3 /GX /O2 /I "../" /I "../paramdesk/" /D "NDEBUG" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ../../libraries/uicore.lib ../../libraries/propgrid.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../binaries/paramdesk.dll" /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "ParamDesk - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "../../intermediate/ParamDesk/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "../" /I "../paramdesk/" /D "_DEBUG" /D "_MBCS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../libraries/uicore.lib ../../libraries/propgrid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../binaries/paramdesk.dll" /pdbtype:sept /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "ParamDesk - Win32 UnicodeDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ParamDesk___Win32_UnicodeDebug"
# PROP BASE Intermediate_Dir "ParamDesk___Win32_UnicodeDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "../../intermediate/ParamDesk/UnicodeDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "_MBCS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "../" /I "../paramdesk/" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../../libraries/uicore.lib ../../libraries/propgrid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../binaries/paramdesk.dll" /pdbtype:sept /libpath:"../../libraries"
# ADD LINK32 ../../libraries/uicore.lib ../../libraries/propgrid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../binaries/paramdesk.dll" /pdbtype:sept /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "ParamDesk - Win32 UnicodeRelease"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ParamDesk___Win32_UnicodeRelease"
# PROP BASE Intermediate_Dir "ParamDesk___Win32_UnicodeRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../libraries"
# PROP Intermediate_Dir "../../intermediate/ParamDesk/UnicodeRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G4 /MD /W3 /GX /O2 /I "../" /D "NDEBUG" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G4 /MD /W3 /GX /O2 /I "../" /I "../paramdesk/" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../../libraries/uicore.lib ../../libraries/propgrid.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../binaries/paramdesk.dll" /libpath:"../../libraries"
# ADD LINK32 ../../libraries/uicore.lib ../../libraries/propgrid.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../binaries/paramdesk.dll" /libpath:"../../libraries"

!ENDIF 

# Begin Target

# Name "ParamDesk - Win32 Release"
# Name "ParamDesk - Win32 Debug"
# Name "ParamDesk - Win32 UnicodeDebug"
# Name "ParamDesk - Win32 UnicodeRelease"
# Begin Group "Resource files"

# PROP Default_Filter ""
# Begin Group "res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resources\res\ParamDesk.rc2
# End Source File
# Begin Source File

SOURCE=.\Resources\res\tab_ctrl_bitmaps.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Resources\ParamDesk.rc
# End Source File
# Begin Source File

SOURCE=.\Resources\Resource.h
# End Source File
# End Group
# Begin Group "params"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\params\ADCCompenPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\ADCCompenPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\AnglesPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\AnglesPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\CarburPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\CarburPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\CKPSPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\CKPSPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\FunSetPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\FunSetPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\IdlRegPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\IdlRegPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\IParamDeskView.h
# End Source File
# Begin Source File

SOURCE=.\params\KnockPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\KnockPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\MiscPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\MiscPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\ParamDeskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\ParamDeskDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\ParamPageEvents.cpp
# End Source File
# Begin Source File

SOURCE=.\params\ParamPageEvents.h
# End Source File
# Begin Source File

SOURCE=.\params\StarterPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\StarterPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\params\TemperPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\params\TemperPageDlg.h
# End Source File
# End Group
# Begin Group "tables"

# PROP Default_Filter ""
# End Group
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Application\ParamDesk.cpp
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
