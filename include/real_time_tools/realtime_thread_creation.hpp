#ifndef REALTIME_THREAD_CREATION_HPP
#define REALTIME_THREAD_CREATION_HPP

#include <string>
#include <vector>

#ifdef  XENOMAI
  // you MAY need to happend "static" upon declaration
  #define THREAD_FUNCTION_RETURN_TYPE void
  #define THREAD_FUNCTION_RETURN_VALUE
  // include xenomai stuff here
#elif defined NON_REAL_TIME
  #include <thread>
  #include <iostream>
  // you need to happend "static" upon declaration
  #define THREAD_FUNCTION_RETURN_TYPE void*
  #define THREAD_FUNCTION_RETURN_VALUE nullptr
#elif defined RT_PREEMPT
  #include <pthread.h>
  #include <limits.h>
  #include <sched.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/mman.h>
  #include <unistd.h>
  // you need to happend "static" upon declaration
  #define THREAD_FUNCTION_RETURN_TYPE void*
  #define THREAD_FUNCTION_RETURN_VALUE nullptr
#endif

namespace real_time_tools {

  // warning : initial version, copy pasted from : https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
  // I did not study things now, so this likely needs improvement
  // (alternative: https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example)
  // note: if failed as mlockall, run executable with sudo or be part of the
  // real_time group or xenomai group.
#ifdef  XENOMAI
  // include xenomai stuff here
#elif defined NON_REAL_TIME
  typedef std::thread RealTimeThread;
#elif defined RT_PREEMPT
  typedef pthread_t RealTimeThread;
#endif


  /**
   * @brief create_realtime_thread spawns a real time thread.
   * based on
   * https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
   * @param[in][out] thread is the c++ object that represents the threads.
   * @param[in] thread_function is the executing function for the thread.
   * @param[in] arguments to be passed to the thread.
   * @param[in] if true, block_memory() will be called at thread creation.
   * @param[in] stack size available to the thread will be factor*PTHREAD_STACK_MIN (rt preempt only)
   * @return the error code.
   */
  int create_realtime_thread(RealTimeThread &thread,
                             void*(*thread_function)(void*),
                             void* args = nullptr,
                             bool call_block_memory = true,
                             int stack_memory_factor=50,
                             std::vector<int> cpu_affinities=std::vector<int>(0));

  /**
   * @brief join_thread join the real time thread
   * @param thread is the C++ thread object to join
   * @return the error code.
   */
  int join_thread(RealTimeThread &thread);

  /**
   * @brief block_memory block the current and futur memory pages
   * see https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/memory#memory-locking
   * for further explanation.
   */
  void block_memory();

} // namespace real_time_tools

#endif // REALTIME_THREAD_CREATION_HPP
