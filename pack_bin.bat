@echo off
rem SECU-3  - An open source, free engine control unit
rem Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
rem 
rem This script archives binaries

if exist binaries (
7z a binaries binaries\*.dll binaries\*.exe binaries\readme.txt binaries\secu3man.sh binaries\COPYRIGHT binaries\secu3man.desktop binaries\sserifer.fon binaries\secu3man.png
) else (
echo Binary files are absent!
exit 1
)
