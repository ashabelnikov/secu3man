@echo off
rem This script archives binaries

if exist binaries (
7z a binaries binaries\*.dll binaries\*.exe
) else (
echo Binary files are absent!
exit 1
)
