# SECU-3  - An open source, free engine control unit
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)

set(CMAKE_MFC_FLAG 2)
set(PROJECT midesk)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

include_directories(../)
project(${PROJECT})

set(HEADERS
  CEDeskDlg.h
  DVDeskDlg.h
  IDVView.h
  IMeasInstrument.h
  IMIView.h
  StdAfx.h
  IRSView.h
  LMDeskDlg.h
  MIAirFlow.h
  MIDeskDlg.h
  MIDwellAngle.h
  MIEGOCorr.h
  MIGasValve.h
  MIHelpers.h
  MIIAT.h
  MIInjPW.h
  MIPressure.h
  MIShutoffValve.h
  MITachometer.h
  MITemperature.h
  MIThrottleGate.h
  MIVoltage.h
  MIVoltMeter.h
  Resource.h
  RSDeskDlg.h
  StdAfx.h
  )

set (SOURCES
  CEDeskDlg.cpp
  DVDeskDlg.cpp
  LMDeskDlg.cpp
  MIAirFlow.cpp
  MIDeskDlg.cpp
  MIDesk.cpp
  MIDwellAngle.cpp
  MIEGOCorr.cpp
  MIGasValve.cpp
  MIIAT.cpp
  MIInjPW.cpp
  MIPressure.cpp
  MIShutoffValve.cpp
  MITachometer.cpp
  MITemperature.cpp
  MIThrottleGate.cpp
  MIVoltage.cpp
  MIVoltMeter.cpp
  RSDeskDlg.cpp
  StdAfx.cpp
  MIDesk.rc
  )

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ui-core)

