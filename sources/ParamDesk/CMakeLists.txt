# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)

set(CMAKE_MFC_FLAG 2)
set(PROJECT paramdesk)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

include_directories(../)
include_directories(../ParamDesk)
project(${PROJECT})

set(HEADERS
  Application/StdAfx.h
  common/ParamPageEvents.h
  common/ParamTabBaseDlg.h
  MAPCalc/MAPCalcController.h
  MAPCalc/MAPCalcDlg.h
  Params/AccelEnrPageDlg.h
  Params/ADCCompenPageDlg.h
  Params/AnglesPageDlg.h
  Params/CarburPageDlg.h
  Params/ChokePageDlg.h
  Params/CKPSPageDlg.h
  Params/FunSetPageDlg.h
  Params/GasdosePageDlg.h
  Params/IdlRegPageDlg.h
  Params/InjectorPageDlg.h
  Params/IParamDeskView.h
  Params/KnockPageDlg.h
  Params/LambdaPageDlg.h
  Params/MiscPageDlg.h
  Params/ParamDeskDlg.h
  Params/SecurPageDlg.h
  Params/StarterPageDlg.h
  Params/TemperPageDlg.h
  Params/UniOutPageDlg.h
  Params/IORemappingDlg.h
  Resources/Resource.h
  Tables/ITablesDeskView.h
  Tables/TablesDeskDlg.h
  Tables/TDContextMenuManager.h
  )

set(SOURCES
  Application/StdAfx.cpp
  Application/ParamDesk.cpp
  common/ParamPageEvents.cpp
  common/ParamTabBaseDlg.cpp
  MAPCalc/MAPCalcController.cpp
  MAPCalc/MAPCalcDlg.cpp
  Params/AccelEnrPageDlg.cpp
  Params/ADCCompenPageDlg.cpp
  Params/AnglesPageDlg.cpp
  Params/CarburPageDlg.cpp
  Params/ChokePageDlg.cpp
  Params/CKPSPageDlg.cpp
  Params/FunSetPageDlg.cpp
  Params/GasdosePageDlg.cpp
  Params/IdlRegPageDlg.cpp
  Params/InjectorPageDlg.cpp
  Params/KnockPageDlg.cpp
  Params/LambdaPageDlg.cpp
  Params/MiscPageDlg.cpp
  Params/ParamDeskDlg.cpp
  Params/SecurPageDlg.cpp
  Params/StarterPageDlg.cpp
  Params/TemperPageDlg.cpp
  Params/UniOutPageDlg.cpp
  Params/IORemappingDlg.cpp
  Tables/TablesDeskDlg.cpp
  Tables/TDContextMenuManager.cpp
  )

set(RESOURCES Resources/ParamDesk.rc )

foreach(src_file ${SOURCES})
  set_source_files_properties(${src_file}
  PROPERTIES COMPILE_FLAGS "/YuStdAfx.h"
  )
endforeach(src_file ${SOURCES})
set_source_files_properties(Application/StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES} ${RESOURCES})
target_link_libraries(${PROJECT} ui-core tabldesk)
