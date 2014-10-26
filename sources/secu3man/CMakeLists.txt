# SECU-3  - An open source, free engine control unit
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

set(PROJECT secu3man)
project(${PROJECT})

ADD_DEFINITIONS(-D_CRT_NON_CONFORMING_SWPRINTFS)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_AFXDLL)
#ADD_DEFINITIONS(-D_UNICODE)

if(MSVC_VERSION GREATER 1299)
ADD_DEFINITIONS(-D_WIN32_WINDOWS=0x0501)
ADD_DEFINITIONS(-DWINVER=0x0501)
endif()

set(LIBRARIES about fwimpexp hexutils hiscctrl io-core midesk
  paramdesk tabldesk ui-core
  )

foreach(LIBRARY ${LIBRARIES})
  add_subdirectory(../${LIBRARY} ../${LIBRARY})
endforeach()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

set(CMAKE_MFC_FLAG 2)
include_directories(../)
include_directories(../secu3man)
include_directories(../ParamDesk)

set(HEADERS
  Application/StdAfx.h
  Application/secu3man.h
  Application/ISECU3man.h
  Application/CommunicationManager.h
  MainFrame/ChildView.h
  MainFrame/ChildViewManager.h
  MainFrame/MainFrame.h
  MainFrame/MainFrameController.h
  MainFrame/MainFrameManager.h
  MainFrame/StatusBarManager.h
  Settings/AppSettingsController.h
  Settings/AppSettingsDlg.h
  Settings/AppSettingsManager.h
  Settings/AppSettingsModel.h
  Settings/IAppSettingsDlg.h
  Settings/ISettingsData.h
  TabControllers/CheckEngineTabController.h
  TabControllers/DevDiagnostTabController.h
  TabControllers/FirmwareFileUtils.h
  TabControllers/FirmwareTabController.h
  TabControllers/FWIORemappingController.h
  TabControllers/FWRPMGridEditController.h
  TabControllers/KnockChannelTabcontroller.h
  Tabcontrollers/LogPlayerTabController.h
  TabControllers/ParamMonTabController.h
  TabControllers/PMControllerBase.h
  TabControllers/PMInitDataCollector.h
  TabControllers/PMMonitorController.h
  TabControllers/PMParamsController.h
  TabControllers/PMTablesController.h
  TabControllers/TabControllersCommunicator.h
  TabDialogs/CheckEngineTabDlg.h
  TabDialogs/DevDiagnostTabDlg.h
  TabDialogs/DiagnostContextMenuManager.h
  TabDialogs/FirmwareContextMenuManager.h
  TabDialogs/FirmwareTabDlg.h
  TabDialogs/KnockChannelTabDlg.h
  TabDialogs/KnockContextMenuManager.h
  TabDialogs/KnockFrqCalcDlg.h
  TabDialogs/LogPlayerTabDlg.h
  TabDialogs/LPControlPanelDlg.h
  TabDialogs/ParamMonTabDlg.h
  TabsManagement/ITabController.h
  TabsManagement/MainTabController.h
  Resources/Resource.h
  )

set(SOURCES
  Application/StdAfx.cpp
  Application/secu3man.cpp
  Application/CommunicationManager.cpp
  Application/CommunicationManager.cpp
  MainFrame/ChildView.cpp
  MainFrame/ChildViewManager.cpp
  MainFrame/MainFrame.cpp
  MainFrame/MainFrameController.cpp
  MainFrame/MainFrameManager.cpp
  MainFrame/StatusBarManager.cpp
  Settings/AppSettingsController.cpp
  Settings/AppSettingsDlg.cpp
  Settings/AppSettingsManager.cpp
  Settings/AppSettingsModel.cpp
  TabControllers/CheckEngineTabController.cpp
  TabControllers/DevDiagnostTabController.cpp
  TabControllers/FirmwareFileUtils.cpp
  TabControllers/FirmwareTabController.cpp
  TabControllers/FWIORemappingController.cpp
  TabControllers/FWRPMGridEditController.cpp
  TabControllers/KnockChannelTabcontroller.cpp
  Tabcontrollers/LogPlayerTabController.cpp
  TabControllers/ParamMonTabController.cpp
  TabControllers/PMInitDataCollector.cpp
  TabControllers/PMMonitorController.cpp
  TabControllers/PMParamsController.cpp
  TabControllers/PMTablesController.cpp
  TabControllers/TabcontrollersCommunicator.cpp
  TabDialogs/CheckEngineTabDlg.cpp
  TabDialogs/DevDiagnostTabDlg.cpp
  TabDialogs/DiagnostContextMenuManager.cpp
  TabDialogs/FirmwareContextMenuManager.cpp
  TabDialogs/FirmwareTabDlg.cpp
  TabDialogs/KnockChannelTabDlg.cpp
  TabDialogs/KnockContextMenuManager.cpp
  TabDialogs/KnockFrqCalcDlg.cpp
  TabDialogs/LogPlayerTabDlg.cpp
  TabDialogs/LPControlPanelDlg.cpp
  TabDialogs/ParamMonTabDlg.cpp
  TabsManagement/MainTabController.cpp
  )

set(RESOURCES Resources/secu3man.rc )

foreach(src_file ${SOURCES})
  set_source_files_properties(${src_file}
  PROPERTIES COMPILE_FLAGS "/YuStdAfx.h"
  )
endforeach(src_file ${SOURCES})
set_source_files_properties(Application/StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")

add_executable(${PROJECT} WIN32 ${HEADERS} ${SOURCES} ${RESOURCES})
target_link_libraries(${PROJECT} ${LIBRARIES})
