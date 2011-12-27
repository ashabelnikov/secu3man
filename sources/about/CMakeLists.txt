cmake_minimum_required(VERSION 2.6) 

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)

ADD_DEFINITIONS(-DABOUT_EXPORTS)

set(PROJECT about) 

#set(LIBRARY_OUTPUT_PATH ../../binaries) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)
 
include_directories(../)
project(${PROJECT}) 
 
set(HEADERS 
  Resource.h
  secu-3about.h
  STCAbout.h 
  StdAfx.h
  )
 
set(SOURCES 
  about.cpp
  secu-3about.cpp
  STCAbout.cpp
  StdAfx.cpp
  about.rc
  ) 

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})