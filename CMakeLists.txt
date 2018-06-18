cmake_minimum_required(VERSION 2.8.3)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project(rtpreempt_tools)

catkin_package(
  LIBRARIES rtpreempt_tools
  INCLUDE_DIRS include
)

include_directories(include)

add_library(rtpreempt_tools src/realtime_check.cpp)

