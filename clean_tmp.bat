@echo off
rem Helpful script for cleaning up all temporary files

call :DelIfExist binaries\*.ilk
call :DelIfExist binaries\*.idb
call :DelIfExist binaries\*.pdb
call :DelIfExist binaries\*.tds
call :DelIfExist libraries\*.lib
call :DelIfExist libraries\*.exp

if exist intermediate (
cd intermediate
call :RDIfExist about\about.dir
call :DelIfExist about\about.ncb
call :RDIfExist io-core\io-core.dir
call :DelIfExist io-core\io-core.ncb
call :RDIfExist midesk\midesk.dir
call :DelIfExist midesk\midesk.ncb
call :RDIfExist ui-core\ui-core.dir
call :DelIfExist ui-core\ui-core.ncb
call :RDIfExist paramdesk\paramdesk.dir
call :DelIfExist paramdesk\paramdesk.ncb
call :RDIfExist fwimpexp\fwimpexp.dir
call :DelIfExist fwimpexp\fwimpexp.ncb
call :RDIfExist hexutils\hexutils.dir
call :DelIfExist hexutils\hexutils.ncb
call :RDIfExist propgrid\propgrid.dir
call :DelIfExist propgrid\propgrid.ncb
call :RDIfExist hiscctrl\hiscctrl.dir
call :DelIfExist hiscctrl\hiscctrl.ncb
call :RDIfExist tabldesk\tabldesk.dir
call :DelIfExist tabldesk\tabldesk.ncb
call :RDIfExist secu3man\secu3man.dir
call :DelIfExist secu3man\secu3man.ncb
cd ..
)

if exist sources (
cd sources
call :DelIfExist Chart2D\*.obj
call :DelIfExist Chart2D\*.~h
call :DelIfExist Chart2D\*.~cpp
call :DelIfExist Chart2D\*.~def
call :DelIfExist Chart2D\*.~bpr
call :DelIfExist Chart2D\*.~bpf
call :DelIfExist Chart2D\*.res
call :DelIfExist Chart3D\*.obj
call :DelIfExist Chart3D\*.~h
call :DelIfExist Chart3D\*.~cpp
call :DelIfExist Chart3D\*.~def
call :DelIfExist Chart3D\*.~bpr
call :DelIfExist Chart3D\*.~bpf
call :DelIfExist Chart3D\*.res
cd ..
)

:DelIfExist
if exist %1 ( DEL /Q %1 )
exit /b

:RDIfExist
if exist %1 ( RD /s/q %1 )
exit /b
