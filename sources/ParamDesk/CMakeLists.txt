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
  Params/ADCCompenPageDlg.h
  Params/AnglesPageDlg.h
  Params/CarburPageDlg.h
  Params/CKPSPageDlg.h
  Params/FunSetPageDlg.h
  Params/IdlRegPageDlg.h
  Params/IParamDeskView.h
  Params/KnockPageDlg.h
  Params/MiscPageDlg.h
  Params/ParamDeskDlg.h
  Params/StarterPageDlg.h
  Params/TemperPageDlg.h
  Resources/Resource.h
  Tables/ITablesDeskView.h
  Tables/TablesDeskDlg.h
  Tables/TablesPageDlg.h
  Tables/TDContextMenuManager.h
  )
 
set(SOURCES 
  Application/StdAfx.cpp
  Application/ParamDesk.cpp
  common/ParamPageEvents.cpp
  Params/ADCCompenPageDlg.cpp
  Params/AnglesPageDlg.cpp
  Params/CarburPageDlg.cpp
  Params/CKPSPageDlg.cpp
  Params/FunSetPageDlg.cpp
  Params/IdlRegPageDlg.cpp
  Params/KnockPageDlg.cpp
  Params/MiscPageDlg.cpp
  Params/ParamDeskDlg.cpp
  Params/StarterPageDlg.cpp
  Params/TemperPageDlg.cpp
  Resources/ParamDesk.rc
  Tables/TablesDeskDlg.cpp
  Tables/TablesPageDlg.cpp
  Tables/TDContextMenuManager.cpp
  ) 

foreach(src_file ${SOURCES})
  set_source_files_properties(${src_file}
  PROPERTIES COMPILE_FLAGS "/YuStdAfx.h"
  )
endforeach(src_file ${SOURCES})
set_source_files_properties(Application/StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
 
add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ui-core propgrid tabldesk)
