cmake_minimum_required(VERSION 2.8.3)

include(cmake/os_detection.cmake)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(rtpreempt_tools)

find_package (catkin)

catkin_package(
  LIBRARIES rtpreempt_tools
  INCLUDE_DIRS include
)

find_package (Threads)

# fill in the CURRENT_OS variable which contains the informations about the
# current operating system we need. It sets respectively the C++ preprocessor
# flags: XENOMAI, RT_PREEMPT, or UBUNTU accordingly.
DEFINE_OS()

include_directories(include)

add_library(rtpreempt_tools
  src/realtime_check.cpp
  src/realtime_thread_creation.cpp
)
target_link_libraries(rtpreempt_tools ${CMAKE_THREAD_LIBS_INIT})

set(RT_CHECK_EX realtime_check_realtime_example)
add_executable(${RT_CHECK_EX}
  src/realtime_check_realtime_example.cpp
)
target_link_libraries(${RT_CHECK_EX}
  rtpreempt_tools
)

set(RT_CHECK_BLOCKING_EX realtime_check_realtime_blocking_example)
add_executable(${RT_CHECK_BLOCKING_EX}
  src/realtime_check_realtime_example_blocking.cpp
)
target_link_libraries(${RT_CHECK_BLOCKING_EX}
  rtpreempt_tools
)
