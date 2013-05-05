# SECU-3  - An open source, free engine control unit
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)
ADD_DEFINITIONS(-DIOCORE_EXPORTS)
ADD_DEFINITIONS(-DCRC_EXPORTS)

set(CMAKE_MFC_FLAG 2)
set(PROJECT io-core)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

include_directories(../)
project(${PROJECT})

set(HEADERS
  BootLoader.h
  BootLoaderAdapter.h
  ccomport.h
  ce_errors.h
  ControlApp.h
  ControlAppAdapter.h
  CRC16.h
  EEPROMDataMediator.h
  FirmwareDataMediator.h
  FirmwareMapsDataHolder.h
  iocore_api.h
  LogReader.h
  LogWriter.h
  NumericConv.h
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
  BootLoader.cpp
  BootLoaderAdapter.cpp
  ccomport.cpp
  ControlApp.cpp
  ControlAppAdapter.cpp
  CRC16.cpp
  EEPROMDataMediator.cpp
  FirmwareDataMediator.cpp
  iocore.cpp
  LogReader.cpp
  LogWriter.cpp
  NumericConv.cpp
  PlatformParamHolder.cpp
  StdAfx.cpp
  )

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
