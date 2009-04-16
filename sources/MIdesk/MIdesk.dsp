# Microsoft Developer Studio Project File - Name="MIdesk" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MIdesk - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MIdesk.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MIdesk.mak" CFG="MIdesk - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MIdesk - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MIdesk - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MIdesk - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G4 /MD /W3 /GX /O2 /I "../" /D "NDEBUG" /D "_MBCS" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 uicore.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../binaries/MIdesk.dll" /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "MIdesk - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 uicore.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../binaries/MIdesk.dll" /pdbtype:sept /libpath:"../../libraries"
# SUBTRACT LINK32 /profile /force

!ENDIF 

# Begin Target

# Name "MIdesk - Win32 Release"
# Name "MIdesk - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MIdesk.cpp
# End Source File
# Begin Source File

SOURCE=.\MIdesk.def
# End Source File
# Begin Source File

SOURCE=.\MIdesk.rc
# End Source File
# Begin Source File

SOURCE=.\MIDeskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RSDeskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\IMIView.h
# End Source File
# Begin Source File

SOURCE=.\IRSView.h
# End Source File
# Begin Source File

SOURCE=.\MIDeskDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RSDeskDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\MIdesk.rc2
# End Source File
# End Group
# Begin Group "instruments"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MeasInstrument.cpp
# End Source File
# Begin Source File

SOURCE=.\MeasInstrument.h
# End Source File
# Begin Source File

SOURCE=.\MIAirFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\MIAirFlow.h
# End Source File
# Begin Source File

SOURCE=.\MIDwellAngle.cpp
# End Source File
# Begin Source File

SOURCE=.\MIDwellAngle.h
# End Source File
# Begin Source File

SOURCE=.\MIGasValve.cpp
# End Source File
# Begin Source File

SOURCE=.\MIGasValve.h
# End Source File
# Begin Source File

SOURCE=.\MIPressure.cpp
# End Source File
# Begin Source File

SOURCE=.\MIPressure.h
# End Source File
# Begin Source File

SOURCE=.\MIShutoffValve.cpp
# End Source File
# Begin Source File

SOURCE=.\MIShutoffValve.h
# End Source File
# Begin Source File

SOURCE=.\MITachometer.cpp
# End Source File
# Begin Source File

SOURCE=.\MITachometer.h
# End Source File
# Begin Source File

SOURCE=.\MITemperature.cpp
# End Source File
# Begin Source File

SOURCE=.\MITemperature.h
# End Source File
# Begin Source File

SOURCE=.\MIThrottleGate.cpp
# End Source File
# Begin Source File

SOURCE=.\MIThrottleGate.h
# End Source File
# Begin Source File

SOURCE=.\MIVoltmeter.cpp
# End Source File
# Begin Source File

SOURCE=.\MIVoltmeter.h
# End Source File
# End Group
# End Target
# End Project
