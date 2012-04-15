cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)

set(CMAKE_MFC_FLAG 2)  
set(PROJECT fwimpexp) 

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)
 
include_directories(../)
project(${PROJECT}) 
 
set(HEADERS 
  EEPROMImpExpController.h
  MapImpExpDlg.h
  MPSZFileDataIO.h
  MPSZImpExpController.h
  Resource.h
  SECUImpExpController.h
  StdAfx.h
  )
 
set(SOURCES 
  EEPROMImpExpController.cpp
  FwImpExp.cpp
  MapImpExpDlg.cpp
  MPSZFileDataIO.cpp
  MPSZImpExpController.cpp
  SECUImpExpController.cpp
  StdAfx.cpp
  FwImpExp.rc
  ) 

add_library (${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} io-core ui-core)
