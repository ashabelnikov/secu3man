@echo off

md intermediate
cd intermediate
md secu3man
cd secu3man

cmake ../../sources/secu3man

cd ..\..\
md libraries

