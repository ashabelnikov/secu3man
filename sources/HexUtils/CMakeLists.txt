cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-DHEXUTILS_EXPORTS)

set(PROJECT hexutils) 

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)
 
project(${PROJECT}) 
 
set(HEADERS 
  readhex.h
  StdAfx.h
  )
 
set(SOURCES 
  HexUtils.cpp
  readhex.cpp
  StdAfx.cpp
  ) 
 
add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
