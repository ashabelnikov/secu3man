#!/bin/sh
# SECU-3  - An open source, free engine control unit
# http://secu-3.org

# Script for building of SECU-3 Manager under Linux.
# Edit it for your needs (VCVARS_PATH variable), copy to the folder containing
# Makefile and run it.

VCVARS_PATH='C:\\Program Files\\Microsoft Visual Studio .NET 2003\\Vc7\\bin\\vcvars32.bat'

if [ $# -gt 1 ]
then
 echo "Invalid number of command line arguments!"
 exit 1
fi

BATCH_FILE=build_wrapper.bat
echo "@echo off" > $BATCH_FILE
echo "call "\"$VCVARS_PATH\" >> $BATCH_FILE
if [ -z "$1" ]
then
 echo "nmake" >> $BATCH_FILE
elif [ $1 = "clean" ]
then
 echo "nmake clean" >> $BATCH_FILE
else
 echo "Invalid command line option!"
exit 1
fi
wine cmd /c $BATCH_FILE
rm -rf $BATCH_FILE