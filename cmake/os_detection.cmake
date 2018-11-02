# _WHICH_OS
# -------------
#
# Execute "uname --all" which provides us information on the os specific
# functions to use
#
macro(DEFINE_OS)
  # Add an option not used yet
  set(CURRENT_OS "ubuntu" CACHE STRING
      "DEPRECATED: Set it to \"xenomai\" or \"rt-preempt\" or \"non-real-time\" to specify the OS")

  # Update submodules as needed
  execute_process(
      COMMAND uname -a
      OUTPUT_VARIABLE UNAME_OUT)
  string(TOLOWER "${UNAME_OUT}" OS_VERSION)


  if(OS_VERSION MATCHES "xenomai")
    set(CURRENT_OS "xenomai")
    add_definitions("-DXENOMAI")
    
  elseif(OS_VERSION MATCHES "preempt rt" OR OS_VERSION MATCHES "preempt-rt")
    set(CURRENT_OS "rt-preempt")
    add_definitions("-DRT_PREEMPT")
    
  elseif(OS_VERSION MATCHES "ubuntu" OR OS_VERSION MATCHES "non-real-time" OR OS_VERSION MATCHES "darwin" )
    set(CURRENT_OS "non-real-time")
    add_definitions("-DNON_REAL_TIME")
  else()
    message(STATUS "output of \"uname -a\": ${OS_VERSION}")
    message(FATAL_ERROR "Could not detect the OS version please "
      "fix os_detection.cmake")
  endif()
  #
  message(STATUS "OS found is " ${CURRENT_OS})
  
  if(OS_VERSION MATCHES "darwin")
    add_definitions("-DMAC_OS")
    message(STATUS "OS found is MAC_OS")
  endif()

endmacro(DEFINE_OS)

