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

del UOZ1\*.obj /Q
del UOZ2\*.obj /Q

cd..






