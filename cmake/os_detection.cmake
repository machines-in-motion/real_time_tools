# _WHICH_OS
# -------------
#
# Execute "uname --all" which provides us information on the os specific
# functions to use
#
macro(DEFINE_OS)
  #
  set(CURRENT_OS "ubuntu" CACHE STRING
      "Set it to \"xenomai\" or \"rt-preempt\" or \"ubuntu\" to specify the OS")
  # Update submodules as needed
  execute_process(
      COMMAND uname --all
      OUTPUT_VARIABLE OS_KERNEL_VERSION)
  #
  if(OS_KERNEL_VERSION MATCHES "xenomai")
    set(CURRENT_OS "xenomai")
    add_definitions("-DXENOMAI")
  elseif(OS_KERNEL_VERSION MATCHES "preempt-rt")
    set(CURRENT_OS "rt-preempt")
    add_definitions("-DRT_PREEMPT")
  elseif(OS_KERNEL_VERSION MATCHES "Ubuntu")
    set(CURRENT_OS "ubuntu")
    add_definitions("-DUBUNTU")
  else()
    message(FATAL_ERROR "Could not detect the OS version please "
      "fix os_detection.cmake")
  endif()
  #
  message(STATUS "OS found is " ${CURRENT_OS})
endmacro(DEFINE_OS)

