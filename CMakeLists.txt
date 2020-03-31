cmake_minimum_required(VERSION 3.1)
project(real_time_tools)

# required to use std::shared_ptr in code and to link the python bindings
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed")
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

find_package(catkin REQUIRED COMPONENTS
  mpi_cmake_modules
  pybind11_catkin
)

search_for_eigen()
set(BOOST_COMPONENTS filesystem)
search_for_boost()
search_for_pthread()

set(LIBRARY_NAME ${PROJECT_NAME})
catkin_package(
  LIBRARIES ${LIBRARY_NAME}
  INCLUDE_DIRS include
  CATKIN_DEPENDS mpi_cmake_modules
  DEPENDS Boost
)

# This macro sets the C++ preprocessor flags "XENOMAI", "RT_PREEMPT", or
# "UBUNTU" according to the current operating system.
DEFINE_OS()

include_directories(include ${catkin_INCLUDE_DIRS})

add_library(${LIBRARY_NAME}
  src/realtime_check.cpp
  src/thread.cpp
  src/spinner.cpp
  src/timer.cpp
  src/iostream.cpp
  src/usb_stream.cpp
  src/process_manager.cpp
  src/frequency_manager.cpp
)

target_link_libraries(${LIBRARY_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  ${Xenomai_LIBS}
)


#########
# DEMOS #
#########
macro(add_real_time_tools_demo source_file)
  add_executable(${source_file} demos/${source_file}.cpp)
  target_link_libraries(${source_file} ${LIBRARY_NAME})
endmacro(add_real_time_tools_demo source_file)

add_real_time_tools_demo(demo_realtime_check)
add_real_time_tools_demo(demo_realtime_strict_check)
add_real_time_tools_demo(demo_spinner)
add_real_time_tools_demo(demo_timing)
add_real_time_tools_demo(demo_thread)
# add_real_time_tools_demo(demo_segfault)
add_real_time_tools_demo(demo_usb_stream_imu_3DM_GX3_25)
add_real_time_tools_demo(demo_checkpoint_timer)

###############
# EXECUTABLES #
###############
#set(EXE1 realtime_test)
#add_executable(${EXE1} src/bin/realtime_test.cpp)
#target_link_libraries(${EXE1} ${LIBRARY_NAME} ${catkin_LIBRARIES})

#set(EXE2 realtime_test_display)
#add_executable(${EXE2} src/bin/realtime_test_display.cpp)
#target_link_libraries(${EXE2} ${LIBRARY_NAME} ${catkin_LIBRARIES})

##################
# Python wrapper #
##################

pybind11_add_module(real_time_tools_py srcpy/wrappers.cpp)
target_link_libraries(real_time_tools_py PRIVATE real_time_tools ${catkin_LIBRARIES})
set_target_properties(real_time_tools_py PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CATKIN_DEVEL_PREFIX}/${CATKIN_GLOBAL_PYTHON_DESTINATION})
install(TARGETS real_time_tools_py DESTINATION ${CATKIN_GLOBAL_PYTHON_DESTINATION})

###################################################
## Set a general config folder path for all tests #
###################################################
set(TEST_CONFIG_PATH ${PROJECT_SOURCE_DIR}/tests/config/)

###################
## add unit tests #
###################

# create the executable
catkin_add_gtest(test_real_time_tools
  tests/main.cpp
  tests/test_real_time_tools.cpp
  tests/threadsafe_object_test.cpp
  tests/threadsafe_timeseries_test.cpp
)
# link the dependecies to it
if (TARGET test_real_time_tools)
  target_link_libraries(test_real_time_tools
    ${LIBRARY_NAME}
    ${catkin_LIBRARIES}
   	${Boost_LIBRARIES}
	)

  # add some preprocessor variable
  target_compile_definitions(test_real_time_tools PUBLIC
    TEST_CONFIG_FOLDER="${TEST_CONFIG_PATH}"
  )
endif()

catkin_install_python(PROGRAMS
  src/bin/compute_statistics
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

##########################
# building documentation #
##########################
build_doxygen_documentation()
