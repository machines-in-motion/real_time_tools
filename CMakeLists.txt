cmake_minimum_required(VERSION 2.8.3)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(rtpreempt_tools)

find_package (catkin)

catkin_package(
  LIBRARIES rtpreempt_tools
  INCLUDE_DIRS include
)

find_package (Threads)

add_definitions("-DRT_PREEMPT")

include_directories(include)

add_library(rtpreempt_tools
  src/realtime_check.cpp
  src/realtime_thread_creation.cpp
)
target_link_libraries(rtpreempt_tools pthread)

set(RT_CHECK_EX realtime_check_realtime_example)
add_executable(${RT_CHECK_EX}
  src/realtime_check_realtime_example.cpp
)
target_link_libraries(${RT_CHECK_EX}
  ${CMAKE_THREAD_LIBS_INIT}
  rtpreempt_tools
)

set(RT_CHECK_BLOCKING_EX realtime_check_realtime_blocking_example)
add_executable(${RT_CHECK_BLOCKING_EX}
  src/realtime_check_realtime_example_blocking.cpp
)
target_link_libraries(${RT_CHECK_BLOCKING_EX}
  ${CMAKE_THREAD_LIBS_INIT}
  rtpreempt_tools
)
