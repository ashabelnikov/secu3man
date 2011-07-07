@ECHO OFF


RD /s/q intermediate

del binaries\*.ilk
del binaries\*.tds
del libraries\*.exp
del libraries\*.pdb

cd sources

del about\*.ncb
del about\*.plg

del io-core\*.ncb
del io-core\*.plg

del midesk\*.ncb
del midesk\*.plg

del ui-core\*.ncb
del ui-core\*.plg

del paramdesk\*.ncb
del paramdesk\*.plg

del secu3man\*.ncb
del secu3man\*.plg

del fwimpexp\*.ncb
del fwimpexp\*.plg

del hexutils\*.ncb
del hexutils\*.plg

del propgrid\*.ncb
del propgrid\*.plg

del hiscctrl\*.ncb
del hiscctrl\*.plg

del tabldesk\*.ncb
del tabldesk\*.plg

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






