#ifndef REALTIME_THREAD_CREATION_HPP
#define REALTIME_THREAD_CREATION_HPP

#include <string>
#include <vector>
#include <memory>

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


/**
 * \page rt_preempt General introduction
 * 
 * \section intro_rtpreempt Introduction
 * warning : initial version, copy pasted from : https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
 * I did not study things now, so this likely needs improvement
 * (alternative: https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example)
 * note: if failed as mlockall, run executable with sudo or be part of the
 * real_time group or xenomai group.
 * 
 * \section block_memory_rtpreempt Block Memory
 * see https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/memory#memory-locking
 * for further explanation.
 */

/**
 * \page xenomai General introduction
 */

/**
 * \page non_real_time General introduction
 */

namespace real_time_tools {

  /**
   * @brief This class is a data structure allowing the user to share
   * configurations among threads. These parameter allows you to generate
   * real threads in xenomai and rt_preempt. The same code is compatible with
   * Mac and ubuntu but will run non-real time threads.
   */
  class RealTimeThreadParameters
  {
  public:
    /**
     * @brief Construct a new RealTimeThreadParameters object
     */
    RealTimeThreadParameters(){
      keyword_ = "real_time_thread";
      priority_ = 80;
#ifdef  XENOMAI
      throw std::runtime_error("xenomai not implemented")
#elif defined NON_REAL_TIME
      stack_size_ = -1;
#elif defined RT_PREEMPT
      stack_size_ = 50*PTHREAD_STACK_MIN;
#endif
      cpu_id_.clear();
      delay_ns_ = 0;
      block_memory_ = true;
    }
    /**
     * @brief Destroy the RealTimeThreadParameters object
     */
    ~RealTimeThreadParameters(){}

  public:
    /**
     * @brief Used in xenomai to define the thread id
     */
    std::string keyword_;
    /**
     * @brief Defines the thread priority from 0 to 100
     */
    int priority_;
    /**
     * @brief Define the stack size
     */
    int stack_size_;
    /**
     * @brief Define the cpu affinity. Which means on which cpu(s) the thread
     * is going to run
     */
    std::vector<int> cpu_id_;
    /**
     * @brief @todo Unknow Xenomai parameter
     */
    int delay_ns_;
    /**
     * @brief Defines if the thread should block the memory in a "page" or if
     * several pages can be use. Switching memory page is time consumming and
     * a non real time operation.
     */
    bool block_memory_;
  };


  /**
   * @brief This class allows you to spawn thread. Its parameter are defined
   * above.
   */
  class RealTimeThread
  {
  public:
    /**
     * @brief Construct a new ThreadInfo object
     */
    RealTimeThread();

    /**
     * @brief The copy constructor only "share the thread". It does not spawn
     * another one.
     */
    RealTimeThread(const real_time_tools::RealTimeThread& other);

    /**
     * @brief Destroy the RealTimeThread object.
     */
    ~RealTimeThread();

    /**
     * @brief create_realtime_thread spawns a real time thread if the OS allows
     * it.
     * @param[in] thread_function: the executing function for the thread.
     * @param[in] args: arguments to be passed to the thread.
     * @return the error code.
     */
    int create_realtime_thread(void*(*thread_function)(void*),
                               void* args = nullptr);

    /**
     * @brief join join the real time thread
     * @param thread is the C++ thread object to join
     * @return the error code.
     */
    int join();

    /**
     * @brief block_memory block the current and futur memory pages.
     */
    void block_memory();

    /**
     * @brief Paramter of the real time thread
     */
    RealTimeThreadParameters parameters_;

  private:
#if defined(XENOMAI)
    throw std::runtime_error("xenomai not implemented")
#elif defined(NON_REAL_TIME)
    std::shared_ptr<std::thread> thread_;
#elif defined(RT_PREEMPT)
    std::shared_ptr<pthread_t> thread_;
#endif
  };
} // namespace real_time_tools

#endif // REALTIME_THREAD_CREATION_HPP
