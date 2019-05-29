/**
 * @file realtime_thread_creation.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Implement method to create and join threads
 */
#include <stdexcept>
#include "real_time_tools/thread.hpp"

namespace real_time_tools {
  RealTimeThread::RealTimeThread()
  {
    thread_.reset(nullptr);
  }

  RealTimeThread::~RealTimeThread()
  {
    join();
    thread_.reset(nullptr);
  }

#if defined RT_PREEMPT

  /**
   * @brief rt_preempt_error_message id common message for all things that could
   * go wrong.
   */
  const std::string rt_preempt_error_message(
        "NOTE: This program must be executed with special permission to get "
        "the required real time permissions.\n"
        "Either use sudo or be part of the \'realtime\' group"
        "Aborting thread creation.");

  int RealTimeThread::create_realtime_thread(
    void*(*thread_function)(void*), void* args)
  {
    if (thread_ != nullptr)
    {
      printf("Thread already running");
    }

    thread_.reset(new pthread_t());

    if(parameters_.block_memory_)
    {
      block_memory();
    }

    struct sched_param param;
    pthread_attr_t attr;
    int ret;

    ret = pthread_attr_init(&attr);
    if (ret) {
      printf("%s %d\n", ("init pthread attributes failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      return ret;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, parameters_.stack_size_);
    if (ret) {
      printf("%s %d\n", ("pthread setstacksize failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      return ret;
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    // ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    if (ret) {
      printf("%s %d\n", ("pthread setschedpolicy failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      return ret;
    }
    param.sched_priority = parameters_.priority_;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
      printf("%s %d\n", ("pthread setschedparam failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      return ret;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
      printf("%s %d\n", ("pthread setinheritsched failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      return ret;
    }

    /* Create a pthread with specified attributes */
    ret = pthread_create(thread_.get(), &attr, thread_function, args);
    if (ret) {
      printf("%s %d\n", ("create pthread failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      return ret;
    }

    if(parameters_.cpu_id_.size()>0)
    {
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      for (unsigned i=0 ; i<parameters_.cpu_id_.size() ; ++i)
      {
        CPU_SET(parameters_.cpu_id_[i], &cpuset);
      }
      ret = pthread_setaffinity_np(*thread_, sizeof(cpu_set_t), &cpuset);
      if (ret)
      {
        printf("%s %d\n", ("Associate thread to a specific cpu failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      }

      int get_aff_error = 0;
      get_aff_error = pthread_getaffinity_np(*thread_, sizeof(cpu_set_t), &cpuset);
      if (get_aff_error)
      {
        printf("%s %d\n", ("Check the thread cpu affinity failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      }
      printf("Set returned by pthread_getaffinity_np() contained: ");
      for (unsigned j = 0; j < CPU_SETSIZE; j++)
      {
        if (CPU_ISSET(j, &cpuset))
        {
          printf("CPU %d, ", j);
        }
      }
      printf("\n");
    }
    return ret;
  }

  int RealTimeThread::join()
  {
    int ret = 0;
    if(thread_ != nullptr)
    {
      /* Join the thread and wait until it is done */
      ret = pthread_join(*thread_, nullptr);
      if (ret)
      {
        printf("join pthread failed.\n");
      }
      thread_.reset(nullptr);
    }
    return ret;
  }

  void RealTimeThread::block_memory()
  {
    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
      printf("mlockall failed: %m\n");
      exit(-2);
    }
  }
#endif // Defined RT_PREEMPT

  /**********************************************************
   * TODO: Check the implementation of this thread creation *
   **********************************************************/
#if defined XENOMAI
  int RealTimeThread::create_realtime_thread(RealTimeThread &thread,
                             void*(*thread_function)(void*),
                             void* args,
                             bool call_block_memory,
			                       int stack_factor,
                             std::vector<int> cpu_affinities{
    int ret=0;
    throw std::runtime_error("create_realtime_thread: "
                             "Please implement this for xenomai");
    return ret;
  }

  int join_thread(RealTimeThread &thread)
  {
    int ret=0;
    throw std::runtime_error("join_thread: "
                             "Please implement this for xenomai");
    return ret;
  }

  void RealTimeThread::block_memory()
  {
    throw std::runtime_error("block_memory: "
                             "Please implement this for xenomai");
  }
#endif // Defined XENOMAI

  /**********************************************************
   * TODO: Check the implementation of this thread creation *
   **********************************************************/
#if defined NON_REAL_TIME

  int RealTimeThread::create_realtime_thread(
    void*(*thread_function)(void*), void* args)
  {
    printf("Warning this thread is not going to be real time.\n");

    /* Create a standard thread for non-real time OS */
    thread_.reset(new std::thread(thread_function, args));
    return 0;
  }

  int RealTimeThread::join()
  {
    if(thread_ != nullptr)
    {
      if(thread_->joinable())
      {
        thread_->join();
      }
    }
    return 0;
  }

  void RealTimeThread::block_memory()
  {
    // do nothing
  }
#endif // Defined NON_REAL_TIME

  /**
   * @brief Construct a new RealTimeThread::RealTimeThread object. It copies the
   * paramters of the other thread but does *NOT* spwan a new one.
   * Used by all os.
   * 
   * @param other 
   */
  RealTimeThread::RealTimeThread(const RealTimeThread& other)
  {
    thread_.reset(nullptr);
    parameters_ = other.parameters_;
  }

} // namespace real_time_tools
