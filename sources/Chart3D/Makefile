# SECU-3  - An open source, free engine control unit
#
# Makefile for building of Chart3D DLL. Borland C Bulder is required.
# Created by Alexey A. Shabelnikov, UA, Kiev. http://secu-3.org

# ----------------------------------------------------------------------------
VERSION = BCB.01
# ----------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ----------------------------------------------------------------------------
PROJECT  = ..\..\binaries\Chart3D.dll
OBJFILES = Form3D.obj Chart3D.obj TChartEx.obj TFloatUpDown.obj PtMovStepDlg.obj
RESDEPEN = $(RESFILES) Form3D.dfm PtMovStepDlg.dfm
RESFILES = resource.res
DEFFILE  = Chart3D.def
LIBFILES =
# ----------------------------------------------------------------------------
CFLAG1 = -Od -Hc -w -k -r- -y -v -vi- -c -a4 -b- -w-par -w-inl -Vx -Ve -x
CFLAG2 = -I$(BCB)\include;$(BCB)\include\vcl -H=$(BCB)\lib\vcld.csm
PFLAGS = -U$(BCB)\lib\obj;$(BCB)\lib \
   -I$(BCB)\include;$(BCB)\include\vcl \
   -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE -v \
   -$Y -$W -$O- -JPHNV -M
RFLAGS = -i$(BCB)\include;$(BCB)\include\vcl 
LFLAGS = -L$(BCB)\lib\obj;$(BCB)\lib -aa -Tpd -x -Gn -Gi -l..\..\libraries
IFLAGS =
LINKER = ilink32
# ----------------------------------------------------------------------------
ALLOBJ = c0d32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) vcl.lib import32.lib cp32mt.lib Tee8C5.lib import32.lib cp32mt.lib
# ----------------------------------------------------------------------------
.autodepend

$(PROJECT): $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES)
!

.pas.hpp:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.pas.obj:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.cpp.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $*

.c.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $**

.rc.res:
    $(BCB)\BIN\brcc32 $(RFLAGS) $<
#------------------------------------------------------------------------------

clean:
    @if exist ..\..\binaries\Chart3D.dll ( DEL /Q ..\..\binaries\Chart3D.dll )
    @if exist ..\..\binaries\Chart3D.tds ( DEL /Q ..\..\binaries\Chart3D.tds )
    @if exist ..\..\libraries\Chart3D.lib ( DEL /Q ..\..\libraries\Chart3D.lib )
    @if exist *.obj ( DEL /Q *.obj )
    @if exist *.res ( DEL /Q *.res )
