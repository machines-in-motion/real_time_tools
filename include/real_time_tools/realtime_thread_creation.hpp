#ifndef REALTIME_THREAD_CREATION_HPP
#define REALTIME_THREAD_CREATION_HPP

#include <string>

#ifdef  XENOMAI
  // include xenomai stuff here
#elif defined NON_REAL_TIME
  #include <thread>
  #include <iostream>
#elif defined RT_PREEMPT
  #include <pthread.h>
  #include <limits.h>
  #include <sched.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/mman.h>
  #include <unistd.h>
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
   * @return the error code.
   */
  int create_realtime_thread(RealTimeThread &thread,
                             void*(*thread_function)(void*),
                             void* args = nullptr);

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
