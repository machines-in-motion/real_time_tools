cmake_minimum_required(VERSION 2.8.3)

include(cmake/os_detection.cmake)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(real_time_tools)

find_package (catkin)

catkin_package(
  LIBRARIES real_time_tools
  INCLUDE_DIRS include
)

find_package (Threads)

# fill in the CURRENT_OS variable which contains the informations about the
# current operating system we need. It sets respectively the C++ preprocessor
# flags: XENOMAI, RT_PREEMPT, or UBUNTU accordingly.
DEFINE_OS()

include_directories(include)

set(LIBRARY_NAME ${PROJECT_NAME})
add_library(${LIBRARY_NAME}
  src/realtime_check.cpp
  src/realtime_thread_creation.cpp
)
target_link_libraries(real_time_tools ${CMAKE_THREAD_LIBS_INIT})

#########
# DEMOS #
#########
set(DEMO1 demo_realtime_check)
add_executable(${DEMO1} demos/demo_realtime_check.cpp)
target_link_libraries(${DEMO1} ${LIBRARY_NAME})

set(DEMO2 demo_realtime_strict_check)
add_executable(${DEMO2} demos/demo_realtime_strict_check.cpp)
target_link_libraries(${DEMO2} ${LIBRARY_NAME})
