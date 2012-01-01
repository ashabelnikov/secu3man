@echo off

del binaries\*.ilk
del binaries\*.idb
del binaries\*.pdb
del binaries\*.tds
del libraries\*.lib
del libraries\*.exp

cd intermediate

RD /s/q about\about.dir
del about\about.ncb

RD /s/q io-core\io-core.dir
del io-core\io-core.ncb

RD /s/q midesk\midesk.dir
del midesk\midesk.ncb

RD /s/q ui-core\ui-core.dir
del ui-core\ui-core.ncb

RD /s/q paramdesk\paramdesk.dir
del paramdesk\paramdesk.ncb

RD /s/q fwimpexp\fwimpexp.dir
del fwimpexp\fwimpexp.ncb

RD /s/q hexutils\hexutils.dir
del hexutils\hexutils.ncb

RD /s/q propgrid\propgrid.dir
del propgrid\propgrid.ncb

RD /s/q hiscctrl\hiscctrl.dir
del hiscctrl\hiscctrl.ncb

RD /s/q tabldesk\tabldesk.dir
del tabldesk\tabldesk.ncb

RD /s/q secu3man\secu3man.dir
del secu3man\secu3man.ncb

cd..

cd sources

del Chart2D\*.obj  /Q
del Chart2D\*.~h   /Q
del Chart2D\*.~cpp /Q
del Chart2D\*.~def /Q
del Chart2D\*.~bpr /Q
del Chart2D\*.~bpf /Q
del Chart2D\*.res  /Q
del Chart3D\*.obj  /Q
del Chart3D\*.~h   /Q
del Chart3D\*.~cpp /Q
del Chart3D\*.~def /Q
del Chart3D\*.~bpr /Q
del Chart3D\*.~bpf /Q
del Chart3D\*.res  /Q

cd..
