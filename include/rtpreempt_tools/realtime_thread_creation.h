#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>


namespace rtpreempt_tools {

  // warning : initial version, copy pasted from : https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
  // I did not study things now, so this likely needs improvement
  // (alternative: https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example)

  // note: if failed as mlockall, run executable with sudo.

  /**
   * @brief create_realtime_thread spawns a real time thread.
   * @param[in][out] thread is the c++ object that represents the threads.
   * @param[in] thread_function is the executing function for the thread.
   * @return the error code.
   */
  int create_realtime_thread(pthread_t &thread,
                             void*(*thread_function)(void*));

  /**
   * @brief create_realtime_thread spawns a real time thread and block the
   * current and future memory
   * @param[in][out] thread is the c++ object that represents the threads.
   * @param[in] thread_function is the executing function for the thread.
   * @return the error code.
   */
  int create_realtime_thread_and_block_memory(pthread_t &thread,
                                              void*(*thread_function)(void*));

  /**
   * @brief join_thread join the real time thread
   * @param thread is the C++ thread object to join
   * @return the error code.
   */
  int join_thread(pthread_t &thread);

  /**
   * @brief block_memory block the current and futur memory pages
   * see https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/memory#memory-locking
   * for further explanation.
   */
  void block_memory();
}
