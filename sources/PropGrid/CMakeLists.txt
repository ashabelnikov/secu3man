cmake_minimum_required(VERSION 2.6) 

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)

set(CMAKE_MFC_FLAG 2)  
set(PROJECT propgrid) 

#set(LIBRARY_OUTPUT_PATH ../../binaries) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)
 
include_directories(../)
project(${PROJECT}) 
 
set(HEADERS 
  DynDialogEx.h
  DynDialogItemEx.h
  ListDynDialogEx.h
  PropertyGrid.h
  PropertyGridCombo.h
  PropertyGridDirectoryPicker.h
  PropertyGridInPlaceEdit.h
  PropertyGridMonthCalCtrl.h
  PropertyGridSelector.h
  Resource.h
  StdAfx.h
  wm_messages.h
  )
 
set(SOURCES 
  DynDialogEx.cpp
  DynDialogItemEx.cpp
  ListDynDialogEx.cpp
  PropGrid.cpp
  PropertyGrid.cpp
  PropertyGridCombo.cpp
  PropertyGridDirectoryPicker.cpp
  PropertyGridInPlaceEdit.cpp
  PropertyGridMonthCalCtrl.cpp
  PropertyGridSelector.cpp
  StdAfx.cpp
  ) 
 
add_library (${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ui-core)
