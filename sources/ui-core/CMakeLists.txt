# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)
ADD_DEFINITIONS(-DDDX_HELPERS_EXPORTS)
ADD_DEFINITIONS(-DFNT_HELPERS_EXPORTS)
ADD_DEFINITIONS(-DXBROWSEFORFOLDER_EXPORTS)

set(CMAKE_MFC_FLAG 2)
set(PROJECT ui-core)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

include_directories(../)
project(${PROJECT})

set(HEADERS
  AnalogMeter.h
  AnalogMeterCtrl.h
  Chart2D.h
  ddx_helpers.h
  fnt_helpers.h
  DialogWithAccelerators.h
  EditEx.h
  HeaderCtrlEx.h
  HotKeysManager.h
  HotKeysToCmdRouter.h
  ITabControllerEvent.h
  Label.h
  LEDIndicator.h
  MapEditorCtrl.h
  OScopeCtrl.h
  ProgressCtrlEx.h
  SpinButtonctrlEx.h
  StatusBarEx.h
  StdAfx.h
  TabController.h
  TabDialog.h
  TextPane.h
  ToolTipCtrlEx.h
  UpdatableDialog.h
  WndScroller.h
  WindowSubClasser.h
  XBrowseForFolder.h
  )

set(SOURCES
  AnalogMeter.cpp
  AnalogMeterCtrl.cpp
  Chart2D.cpp
  ddx_helpers.cpp
  fnt_helpers.cpp
  DialogWithAccelerators.cpp
  EditEx.cpp
  HeaderCtrlEx.cpp
  HotKeysManager.cpp
  HotKeysToCmdRouter.cpp
  ITabControllerEvent.cpp
  Label.cpp
  LEDIndicator.cpp
  MapEditorCtrl.cpp
  OScopeCtrl.cpp
  ProgressCtrlEx.cpp
  SpinButtonctrlEx.cpp
  StatusBarEx.cpp
  StdAfx.cpp
  TabController.cpp
  TabDialog.cpp
  TextPane.cpp
  ToolTipCtrlEx.cpp
  UpdatableDialog.cpp
  WndScroller.cpp
  WindowSubClasser.cpp
  XBrowseForFolder.cpp
  uicore.cpp
  uicore.rc
  )

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
