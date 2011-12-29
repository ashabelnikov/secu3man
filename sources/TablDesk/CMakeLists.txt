cmake_minimum_required(VERSION 2.6) 

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)
ADD_DEFINITIONS(-DDLLLF_EXPORTS)

set(CMAKE_MFC_FLAG 2)  
set(PROJECT tabldesk) 

#set(LIBRARY_OUTPUT_PATH ../../binaries) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)
 
include_directories(../)
project(${PROJECT}) 
 
set(HEADERS 
  ButtonsPanel.h
  DLLLinkedFunctions.h
  MapIds.h
  resource.h
  StdAfx.h
  TablesSetPanel.h
  )
 
set(SOURCES 
  ButtonsPanel.cpp
  DLLLinkedFunctions.cpp
  TablesSetPanel.cpp
  StdAfx.cpp
  TablDesk.cpp
  TablDesk.rc
  ) 
 
add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
