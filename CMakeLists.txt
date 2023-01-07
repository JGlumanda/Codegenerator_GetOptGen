cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")

project("DHBW CodeGenerator")

# Search the project binary dir for config.h
add_executable(target header/write.h header/read.h source/main.cpp source/write.cpp source/read.cpp)

install(
  TARGETS ${sample_programs}
  RUNTIME DESTINATION "build"
  COMPONENT "runtime")
