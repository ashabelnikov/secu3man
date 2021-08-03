# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
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
  ContextMenuManager.h
  DVDeskDlg.h
  IDVView.h
  MeasInstrBase.h
  IMIView.h
  StdAfx.h
  IRSView.h
  LMDeskDlg.h
  MIAirFlow.h
  MIDeskDlg.h
  MIDwellAngle.h
  MIEGOCorr.h
  MIIAT.h
  MIInjPW.h
  MIInjTim.h
  MIInjDuty.h
  MIPressure.h
  MITachometer.h
  MITemperature.h
  MIThrottleGate.h
  MIVoltage.h
  MIVoltMeter.h
  MIVehicleSpeed.h
  MITPSDot.h
  MIMap2.h
  MIMapD.h
  MIMaf.h
  MITemp2.h
  MIGrts.h
  MIFuelConsum.h
  MIFuelConsumF.h
  MIKnockRetard.h
  MIKnockGraph.h
  MISensAFR.h
  MIChokePos.h
  MISynLoad.h
  MIFtls.h
  MIEgts.h
  MIOps.h
  Resource.h
  RSDeskDlg.h
  RingBuffer.h
  StdAfx.h
  )

set (SOURCES
  CEDeskDlg.cpp
  ContextMenuManager.cpp
  DVDeskDlg.cpp
  LMDeskDlg.cpp
  MIAirFlow.cpp
  MIDeskDlg.cpp
  MIDesk.cpp
  MIDwellAngle.cpp
  MIEGOCorr.cpp
  MIIAT.cpp
  MIInjPW.cpp
  MIInjTim.cpp
  MIInjDuty.cpp
  MIPressure.cpp
  MITachometer.cpp
  MITemperature.cpp
  MIThrottleGate.cpp
  MIVoltage.cpp
  MIVoltMeter.cpp
  MIVehicleSpeed.cpp
  MITPSDot.cpp
  MIMap2.cpp
  MIMapD.cpp
  MIMaf.cpp
  MITemp2.cpp
  MIGrts.cpp
  MIFuelConsum.cpp
  MIFuelConsumF.cpp
  MIKnockRetard.cpp
  MIKnockGraph.cpp
  MISensAFR.cpp
  MIChokePos.cpp
  MIFtls.cpp
  MIEgts.cpp
  MIOps.cpp
  MISynLoad.cpp
  RSDeskDlg.cpp
  StdAfx.cpp
  MIDesk.rc
  )

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ui-core io-core)

