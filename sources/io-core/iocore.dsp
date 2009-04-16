# Microsoft Developer Studio Project File - Name="iocore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=iocore - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iocore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iocore.mak" CFG="iocore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iocore - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "iocore - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iocore - Win32 Release"

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
# ADD CPP /nologo /G4 /MD /W3 /GX /O2 /I "../" /D "NDEBUG" /D "_MBCS" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /D "CRC_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"../../binaries/iocore.dll" /libpath:"../../libraries"

!ELSEIF  "$(CFG)" == "iocore - Win32 Debug"

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
# ADD CPP /nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "_WINDLL" /D "_AFXDLL" /D "CRC_EXPORTS" /D "_MBCS" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../binaries/iocore.dll" /pdbtype:sept /libpath:"../../libraries"

!ENDIF 

# Begin Target

# Name "iocore - Win32 Release"
# Name "iocore - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BootLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\BootLoaderAdapter.cpp
# End Source File
# Begin Source File

SOURCE=.\ccomport.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlApp.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlAppAdapter.cpp
# End Source File
# Begin Source File

SOURCE=.\CRC16.cpp
# End Source File
# Begin Source File

SOURCE=.\EEPROMDataMediator.cpp
# End Source File
# Begin Source File

SOURCE=.\FirmwareDataMediator.cpp
# End Source File
# Begin Source File

SOURCE=.\iocore.cpp
# End Source File
# Begin Source File

SOURCE=.\iocore.def
# End Source File
# Begin Source File

SOURCE=.\iocore.rc
# End Source File
# Begin Source File

SOURCE=.\LogWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\NumericConv.cpp
# End Source File
# Begin Source File

SOURCE=.\SECU3IO.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BootLoader.h
# End Source File
# Begin Source File

SOURCE=.\BootLoaderAdapter.h
# End Source File
# Begin Source File

SOURCE=.\ccomport.h
# End Source File
# Begin Source File

SOURCE=.\ce_errors.h
# End Source File
# Begin Source File

SOURCE=.\ControlApp.h
# End Source File
# Begin Source File

SOURCE=.\ControlAppAdapter.h
# End Source File
# Begin Source File

SOURCE=.\CRC16.h
# End Source File
# Begin Source File

SOURCE=.\EEPROMDataMediator.h
# End Source File
# Begin Source File

SOURCE=.\FirmwareDataMediator.h
# End Source File
# Begin Source File

SOURCE=.\FirmwareMapsDataHolder.h
# End Source File
# Begin Source File

SOURCE=.\LogWriter.h
# End Source File
# Begin Source File

SOURCE=.\NumericConv.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\ufcodes.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\iocore.rc2
# End Source File
# End Group
# End Target
# End Project
