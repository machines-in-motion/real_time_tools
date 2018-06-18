cmake_minimum_required(VERSION 2.8.3)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project(rtpreempt_tools)

catkin_package(
  LIBRARIES rtpreempt_tools
  INCLUDE_DIRS include
)

find_package (Threads)


include_directories(include)

add_library(rtpreempt_tools src/realtime_check.cpp)

add_executable(realtime_check_realtime_example src/realtime_check_realtime_example.cpp)
target_link_libraries(realtime_check_realtime_example ${CMAKE_THREAD_LIBS_INIT} rtpreempt_tools)

