# SECU-3  - An open source, free engine control unit
#
# Makefile for building of ChartxD DLL. Borland C Bulder is required.
# Created by Alexey A. Shabelnikov, UA, Kiev. http://secu-3.org

# ----------------------------------------------------------------------------
VERSION = BCB.01
# ----------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ----------------------------------------------------------------------------
PROJECT  = ..\..\binaries\ChartxD.dll
OBJFILES = ManageFrm.obj ChartxD.obj Chart2D\Form2D.obj Chart2D\Chart2D.obj Chart3D\Form3D.obj Chart3D\Chart3D.obj TChartEx.obj TFloatUpDown.obj PtMovStep\PtMovStepDlg.obj Chart3D\Selection.obj
RESDEPEN = $(RESFILES) Chart2D\Form2D.dfm Chart3D\Form3D.dfm PtMovStep\PtMovStepDlg.dfm
RESFILES = Chart2D\resource.res Chart3D\resource.res PtMovStep\resource.res resource.res
DEFFILE  = ChartxD.def
LIBFILES =
# ----------------------------------------------------------------------------
CFLAG1 = -Od -Hc -w -k -r- -y -v -vi- -c -a4 -b- -w-par -w-inl -Vx -Ve -x
CFLAG2 = -I$(BCB)\include;$(BCB)\include\vcl -H=$(BCB)\lib\vcld.csm
PFLAGS = -U$(BCB)\lib\obj;$(BCB)\lib \
   -I$(BCB)\include;$(BCB)\include\vcl \
   -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE -v \
   -$Y -$W -$O- -JPHNV -M
RFLAGS = -i$(BCB)\include;$(BCB)\include\vcl 
LFLAGS = -L$(BCB)\lib\obj;$(BCB)\lib;Chart2D;Chart3D;PtMovStep -aa -Tpd -x -Gn -Gi -l..\..\libraries
IFLAGS =
LINKER = ilink32
# ----------------------------------------------------------------------------
ALLOBJ = c0d32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) vcl.lib import32.lib cp32mt.lib Tee8C5.lib import32.lib cp32mt.lib TeePro8C5.lib
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
    @if exist Chart2D\*.obj ( DEL /Q Chart2D\*.obj )
    @if exist Chart3D\*.obj ( DEL /Q Chart3D\*.obj )
    @if exist PtMovStep\*.obj ( DEL /Q PtMovStep\*.obj )
    @if exist *.res ( DEL /Q *.res )
    @if exist Chart2D\*.res ( DEL /Q Chart2D\*.res )
    @if exist Chart3D\*.res ( DEL /Q Chart3D\*.res )
    @if exist PtMovStep\*.res ( DEL /Q PtMovStep\*.res )
