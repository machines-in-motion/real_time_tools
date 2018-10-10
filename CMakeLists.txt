cmake_minimum_required(VERSION 2.8.3)

include(cmake/os_detection.cmake)

# required to use std::shared_ptr in code and to link the python bindings
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--no-as-needed")
else()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

project(real_time_tools)

find_package (catkin)

catkin_package(
  LIBRARIES real_time_tools
  INCLUDE_DIRS include
  CFG_EXTRAS os_detection.cmake
)

find_package (Threads)

# This macro sets the C++ preprocessor flags "XENOMAI", "RT_PREEMPT", or
# "UBUNTU" according to the current operating system.
DEFINE_OS()

include_directories(include)

set(LIBRARY_NAME ${PROJECT_NAME})
add_library(${LIBRARY_NAME}
  src/realtime_check.cpp
  src/realtime_thread_creation.cpp
  src/spinner.cpp
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

set(DEMO3 demo_spinner)
add_executable(${DEMO3} demos/demo_spinner.cpp)
target_link_libraries(${DEMO3} ${LIBRARY_NAME})


############################
# install the cmake macros #
############################
configure_file(cmake/os_detection.cmake
  ${CATKIN_DEVEL_PREFIX}/${CATKIN_GLOBAL_SHARE_DESTINATION}/cmake/os_detection.cmake
  COPYONLY)
