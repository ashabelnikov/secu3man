# SECU-3  - An open source, free engine control unit
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)

set(CMAKE_MFC_FLAG 2)
set(PROJECT hiscctrl)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

include_directories(../)
project(${PROJECT})

set(HEADERS
  StdAfx.h
  sources/ChartAxis.h
  sources/ChartAxisLabel.h
  sources/ChartBarSerie.h
  sources/ChartCtrl.h
  sources/ChartGradient.h
  sources/ChartGrid.h
  sources/ChartLegend.h
  sources/ChartLineSerie.h
  sources/ChartObject.h
  sources/ChartPointsSerie.h
  sources/ChartScrollBar.h
  sources/ChartSerie.h
  sources/ChartSurfaceSerie.h
  sources/ChartTitle.h
  )

set(SOURCES
  StdAfx.cpp
  sources/ChartAxis.cpp
  sources/ChartAxisLabel.cpp
  sources/ChartBarSerie.cpp
  sources/ChartCtrl.cpp
  sources/ChartGradient.cpp
  sources/ChartGrid.cpp
  sources/ChartLegend.cpp
  sources/ChartLineSerie.cpp
  sources/ChartObject.cpp
  sources/ChartPointsSerie.cpp
  sources/ChartScrollBar.cpp
  sources/ChartSerie.cpp
  sources/ChartSurfaceSerie.cpp
  sources/ChartTitle.cpp
  hiscctrl.rc
  )

foreach(src_file ${SOURCES})
  set_source_files_properties(${src_file}
  PROPERTIES COMPILE_FLAGS "/YuStdAfx.h"
  )
endforeach(src_file ${SOURCES})
set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
