#
# Copyright (c) 2019, New York University and Max Planck Gesellschaft.
#
# License BSD-3 clause
#

#
# set up the project
#
cmake_minimum_required(VERSION 3.10.2)

project(real_time_tools VERSION 2.0.0)

# Using C++17
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

#
# Dependencies
#

# Usual dependencies
find_package(mpi_cmake_modules REQUIRED)
find_package(pybind11 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system thread)
find_package(Threads REQUIRED)

#
# Setup the environment from dependencies
#

# This macro sets the C++ preprocessor flags "XENOMAI", "RT_PREEMPT", or
# "UBUNTU" according to the current operating system.
define_os()
set(all_targets)

#
# Add the main library
#

# Create the CMake target
add_library(
  ${PROJECT_NAME} SHARED
  src/realtime_check.cpp
  src/thread.cpp
  src/spinner.cpp
  src/timer.cpp
  src/iostream.cpp
  src/usb_stream.cpp
  src/process_manager.cpp
  src/frequency_manager.cpp)
# Add the include dependencies
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)
# Link the dependencies
target_link_libraries(${PROJECT_NAME} Boost::boost Boost::filesystem
                      Boost::system Boost::thread)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
# Xenomai libs could be empty. But this is needed in case the OS is Xenomai
target_link_libraries(${PROJECT_NAME} ${Xenomai_LIBS})
# For the installation
list(APPEND all_targets ${PROJECT_NAME})

#
# Demos.
#
macro(add_real_time_tools_demo demo_name)
  # set(demo_target_name ${PROJECT_NAME}_${demo_name})
  set(demo_target_name ${demo_name})
  # add the executable.
  add_executable(${demo_target_name} demos/${demo_name}.cpp)
  # save the name for installation
  set(all_targets ${all_targets} ${demo_target_name})
  # dependencies
  target_include_directories(
    ${demo_target_name}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)
  target_link_libraries(${demo_target_name} ${PROJECT_NAME})
endmacro(add_real_time_tools_demo target_name)

add_real_time_tools_demo(demo_realtime_check)
add_real_time_tools_demo(demo_realtime_strict_check)
add_real_time_tools_demo(demo_spinner)
add_real_time_tools_demo(demo_timing)
add_real_time_tools_demo(demo_thread)
add_real_time_tools_demo(demo_usb_stream_imu_3DM_GX3_25)
add_real_time_tools_demo(demo_checkpoint_timer)

#
# Executables.
#

# set(EXE1 realtime_test) add_executable(${EXE1} src/bin/realtime_test.cpp)
# target_link_libraries(${EXE1} ${PROJECT_NAME} ${catkin_LIBRARIES})

# set(EXE2 realtime_test_display) add_executable(${EXE2}
# src/bin/realtime_test_display.cpp) target_link_libraries(${EXE2}
# ${PROJECT_NAME} ${catkin_LIBRARIES})

#
# Python wrapper.
#
add_library(${PROJECT_NAME}_cpp_bindings MODULE srcpy/real_time_tools.cpp)
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE pybind11::module)
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE ${PYTHON_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_cpp_bindings
                      PROPERTIES PREFIX "" SUFFIX "${PYTHON_MODULE_EXTENSION}")
target_include_directories(
  ${PROJECT_NAME}_cpp_bindings
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include> SYSTEM
  PUBLIC ${PYTHON_INCLUDE_DIRS})
get_python_install_dir(python_install_dir)
install(TARGETS ${PROJECT_NAME}_cpp_bindings DESTINATION ${python_install_dir})
install(FILES srcpy/__init__.py
        DESTINATION ${python_install_dir}/${PROJECT_NAME})

#
# Add unit tests.
#
include(CTest)
if(BUILD_TESTING)
  # C++ unit-tests framework used.
  find_package(GTest CONFIG REQUIRED)
  include(GoogleTest)
  # Config folder
  get_filename_component(CONFIG_PATH config ABSOLUTE)
  # create the executable
  add_executable(
    test_real_time_tools
    tests/main.cpp tests/test_real_time_tools.cpp
    tests/threadsafe_object_test.cpp)
  # link to the created librairies and its dependencies
  target_link_libraries(test_real_time_tools ${PROJECT_NAME} GTest::gtest)
  # create 2 macro: the path to the config file and the path to the data file
  target_compile_definitions(
    test_real_time_tools
    PUBLIC
    PUBLIC TEST_CONFIG_FOLDER="${TEST_CONFIG_PATH}")
  # declare the test as gtest
  gtest_add_tests(TARGET test_real_time_tools)
endif()

#
# building documentation
#
add_documentation()

#
# Install the package
#

# install the include directory
install(DIRECTORY include/ DESTINATION include)

# command to install the library and binaries
install(
  TARGETS ${all_targets}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES
  DESTINATION include)

# Export this package as a cmake package.
generate_cmake_package()
