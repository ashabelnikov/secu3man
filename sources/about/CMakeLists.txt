# SECU-3  - An open source, free engine control unit
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)
ADD_DEFINITIONS(-DABOUT_EXPORTS)

set(CMAKE_MFC_FLAG 2)
set(PROJECT about)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

include_directories(../)
set(LIBRARIES Version.lib)
project(${PROJECT})

set(HEADERS
  about_api.h
  Resource.h
  secu-3about.h
  STCAbout.h
  StdAfx.h
  Version.h
  )

set(SOURCES
  about.cpp
  secu-3about.cpp
  STCAbout.cpp
  StdAfx.cpp
  about.rc
  Version.cpp
  )

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${LIBRARIES})
