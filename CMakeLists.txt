cmake_minimum_required(VERSION 2.8.3)

# required to use std::shared_ptr in code and to link the python bindings
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--no-as-needed")
else()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

project(real_time_tools)

find_package(catkin REQUIRED COMPONENTS
    mpi_cmake_modules
)

search_for_eigen()
search_for_boost()
search_for_pthread()

set(LIBRARY_NAME ${PROJECT_NAME})
catkin_package(
  LIBRARIES ${LIBRARY_NAME}
  INCLUDE_DIRS include
  CATKIN_DEPENDS mpi_cmake_modules
)

# This macro sets the C++ preprocessor flags "XENOMAI", "RT_PREEMPT", or
# "UBUNTU" according to the current operating system.
DEFINE_OS()

include_directories(include ${catkin_INCLUDE_DIRS})

add_library(${LIBRARY_NAME}
  src/realtime_check.cpp
  src/realtime_thread_creation.cpp

  src/spinner.cpp
  src/timer.cpp
  src/realtime_iostream.cpp
)

target_link_libraries(real_time_tools
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
)


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

add_executable(demo_timing demos/demo_timing.cpp)
target_link_libraries(demo_timing ${LIBRARY_NAME})

add_executable(demo_segfault demos/demo_segfault.cpp)
target_link_libraries(demo_segfault ${LIBRARY_NAME})



###############
# EXECUTABLES #
###############
#set(EXE1 realtime_test)
#add_executable(${EXE1} src/bin/realtime_test.cpp)
#target_link_libraries(${EXE1} ${LIBRARY_NAME} ${catkin_LIBRARIES})

#set(EXE2 realtime_test_display)
#add_executable(${EXE2} src/bin/realtime_test_display.cpp)
#target_link_libraries(${EXE2} ${LIBRARY_NAME} ${catkin_LIBRARIES})

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
)
# link the dependecies to it
if (TARGET test_real_time_tools)

    target_link_libraries(test_real_time_tools
	real_time_tools
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

###############################################
# Add custom (non compiling) targets          #
# so non cpp files show up in QT_CREATOR IDE. #
###############################################
set(USE_QTCREATOR OFF CACHE BOOL
    "Set to ON if you want QtCreator to display all files in the project tree")
if(USE_QTCREATOR)
    file(GLOB_RECURSE EXTRA_FILES
        ../*.h ../*.hh ../*.hpp ../*.py
        ../*.cf  ../*.yaml ../*.txt ../*.xml
        ../start ../*.launch ../*.urdf ../CATKIN_IGNORE ../*.in
    )
    #list(FILTER EXTRA_FILES EXCLUDE REGEX "build")
    add_custom_target(${PROJECT_NAME}_OTHER_FILES ALL WORKING_DIRECTORY
        ${PROJECT_SOURCE_DIR} SOURCES ${EXTRA_FILES})
endif()
