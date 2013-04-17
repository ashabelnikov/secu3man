@echo off
rem This script configures build environment.

md intermediate
cd intermediate
md secu3man
cd secu3man

cmake ../../sources/secu3man
rem cmake -G "Visual Studio 7 .NET 2003" ../../sources/secu3man
rem cmake -G "NMake Makefiles" ../../sources/secu3man

cd ..\..\
md libraries

md binaries
7z x binaries.7z
