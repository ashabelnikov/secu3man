@echo off
rem Helpful script for cleaning up all temporary folders and files.

call :RDIfExist intermediate
call :RDIfExist libraries
call :RDIfExist binaries

:RDIfExist
if exist %1 ( RD /s/q %1 )
exit /b
