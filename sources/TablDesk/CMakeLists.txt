# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
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
  AutoTuneController.h
  ButtonsPanel.h
  CESettingsDlg.h
  DLLLinkedFunctions.h
  DwellCalcDlg.h
  DynamicValues.h
  GridModeEditorIgnDlg.h
  GridModeEditorInjDlg.h
  GMEInjVEDlg.h
  GMEInjAFRDlg.h
  GMEInjITDlg.h
  GMEInjIRegDlg.h
  GMEInjEnrDlg.h
  GMEInjOtherDlg.h
  GMEInjOther1Dlg.h
  GMEInjPwm1Dlg.h
  GMEInjPwm2Dlg.h
  MapIds.h
  resource.h
  RPMGridEditDlg.h
  StdAfx.h
  TablesSetPanel.h
  )

set(SOURCES
  AutoTuneController.cpp
  ButtonsPanel.cpp
  CESettingsDlg.cpp
  DLLLinkedFunctions.cpp
  DwellCalcDlg.cpp
  GridModeEditorIgnDlg.cpp
  GridModeEditorInjDlg.cpp
  GMEInjVEDlg.cpp
  GMEInjAFRDlg.cpp
  GMEInjITDlg.cpp
  GMEInjIRegDlg.cpp
  GMEInjEnrDlg.cpp
  GMEInjOtherDlg.cpp
  GMEInjOther1Dlg.cpp
  GMEInjPwm1Dlg.cpp
  GMEInjPwm2Dlg.cpp
  RPMGridEditDlg.cpp
  StdAfx.cpp
  TablDesk.cpp
  TablDesk.rc
  TablesSetPanel.cpp
  )

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ui-core)
