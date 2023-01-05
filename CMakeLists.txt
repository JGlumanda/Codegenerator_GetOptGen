cmake_minimum_required(VERSION 3.24)
project(Codegenerator_GetOptGen VERSION 1.0
        DESCRIPTION "GetOpt Generator"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(XercesC_INCLUDE_DIR C:/xerces-c/include)
set(XercesC_LIBRARY C:/xerces-c/lib)

include_directories(.)

add_executable(Codegenerator_GetOptGen
        main.cpp
        read.cpp
        read.h
        write.cpp
        write.h)

find_package(XercesC REQUIRED)
include_directories(${XercesC_INCLUDE_DIR})
target_link_libraries(Codegenerator_GetOptGen ${Xerces_LIBRARY})