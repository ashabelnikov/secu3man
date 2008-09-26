@ECHO OFF

del binaries\*.ilk
del binaries\*.tds
del libraries\*.exp
del libraries\*.pdb

cd sources

del about\debug\*.* /Q
del about\release\*.* /Q
del about\*.ncb
del about\*.plg

del io-core\debug\*.* /Q
del io-core\release\*.* /Q
del io-core\*.ncb
del io-core\*.plg

del midesk\debug\*.* /Q
del midesk\release\*.* /Q
del midesk\*.ncb
del midesk\*.plg

del ui-core\debug\*.* /Q
del ui-core\release\*.* /Q
del ui-core\*.ncb
del ui-core\*.plg

del paramdesk\debug\*.* /Q
del paramdesk\release\*.* /Q
del paramdesk\*.ncb
del paramdesk\*.plg

del secu3man\debug\*.* /Q
del secu3man\release\*.* /Q
del secu3man\*.ncb
del secu3man\*.plg

del fwimpexp\debug\*.* /Q
del fwimpexp\release\*.* /Q
del fwimpexp\*.ncb
del fwimpexp\*.plg

del UOZ1\*.obj /Q
del UOZ2\*.obj /Q

cd..






