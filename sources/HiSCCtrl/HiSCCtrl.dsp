# Microsoft Developer Studio Project File - Name="HiSCCtrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=HiSCCtrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HiSCCtrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HiSCCtrl.mak" CFG="HiSCCtrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HiSCCtrl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HiSCCtrl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HiSCCtrl - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"../../binaries/HiSCCtrl.dll" /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "HiSCCtrl - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ  /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ    /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../binaries/HiSCCtrl.dll" /pdbtype:sept /libpath:"../../libraries"

!ENDIF 

# Begin Target

# Name "HiSCCtrl - Win32 Release"
# Name "HiSCCtrl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\HiSCCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\HiSCCtrl.def
# End Source File
# Begin Source File

SOURCE=.\HiSCCtrl.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\HiSCCtrl.rc2
# End Source File
# End Group
# Begin Group "sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sources\ChartAxis.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartAxis.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartAxisLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartAxisLabel.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartBarSerie.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartBarSerie.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartCtrl.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartGradient.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartGradient.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartGrid.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartLegend.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartLegend.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartLineSerie.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartLineSerie.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartObject.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartObject.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartPointsSerie.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartPointsSerie.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartSerie.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartSerie.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartString.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartSurfaceSerie.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartSurfaceSerie.h
# End Source File
# Begin Source File

SOURCE=.\sources\ChartTitle.cpp
# End Source File
# Begin Source File

SOURCE=.\sources\ChartTitle.h
# End Source File
# End Group
# End Target
# End Project
