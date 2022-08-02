# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)
ADD_DEFINITIONS(-DIOCORE_EXPORTS)
ADD_DEFINITIONS(-DCRC_EXPORTS)
ADD_DEFINITIONS(-DBITMASK_EXPORTS)
ADD_DEFINITIONS(-DPARAMSIO_EXPORTS)
ADD_DEFINITIONS(-DENUMPORTS_EXPORTS)
ADD_DEFINITIONS(-DCOPTDESCR_EXPORTS)

set(CMAKE_MFC_FLAG 2)
set(PROJECT io-core)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

include_directories(../)
set(LIBRARIES setupapi.lib)

project(${PROJECT})

set(HEADERS
  BitMask.h
  BootLoader.h
  BootLoaderAdapter.h
  ccomport.h
  ce_errors.h
  ControlApp.h
  ControlAppAdapter.h
  CoptDescr.h
  CRC16.h
  EEPROMDataMediator.h
  EnumPorts.h
  FirmwareDataMediator.h
  FirmwareMapsDataHolder.h
  iocore_api.h
  LogReader.h
  LogWriter.h
  NumericConv.h
  PacketDataProxy.h
  ParamsIO.h
  PlatformParamHolder.h
  SECU3IO.h
  SECU3ParametersDef.h
  SECU3TablesDef.h
  SECU3Types.h
  types.h
  ufcodes.h
  StdAfx.h
  )

set(SOURCES
  BitMask.cpp
  BootLoader.cpp
  BootLoaderAdapter.cpp
  ccomport.cpp
  ControlApp.cpp
  ControlAppAdapter.cpp
  CoptDescr.cpp
  CRC16.cpp
  EEPROMDataMediator.cpp
  EnumPorts.cpp
  FirmwareDataMediator.cpp
  iocore.cpp
  LogReader.cpp
  LogWriter.cpp
  NumericConv.cpp
  PacketDataProxy.cpp
  ParamsIO.cpp
  PlatformParamHolder.cpp
  StdAfx.cpp
  iocore.rc
  )

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${LIBRARIES})

