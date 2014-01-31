# SECU-3  - An open source, free engine control unit
# http://secu-3.org

cmake_minimum_required(VERSION 2.6) 

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)
ADD_DEFINITIONS(-DDLLLF_EXPORTS)

set(CMAKE_MFC_FLAG 2)  
set(PROJECT tabldesk) 

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)
 
include_directories(../)
project(${PROJECT}) 
 
set(HEADERS 
  ButtonsPanel.h
  DLLLinkedFunctions.h
  DwellCalcDlg.h
  GridModeEditorDlg.h
  MapIds.h
  resource.h
  RPMGridEditDlg.h
  StdAfx.h
  TablesSetPanel.h
  )
 
set(SOURCES 
  ButtonsPanel.cpp
  DLLLinkedFunctions.cpp
  DwellCalcDlg.cpp
  GridModeEditorDlg.cpp
  RPMGridEditDlg.cpp
  StdAfx.cpp
  TablDesk.cpp
  TablDesk.rc
  TablesSetPanel.cpp
  ) 
 
add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ui-core)
