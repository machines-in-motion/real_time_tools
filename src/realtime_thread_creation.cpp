#include <stdexcept>
#include "real_time_tools/realtime_thread_creation.hpp"

namespace real_time_tools {

  /****************************************
   * RT PREEMPT REAL TIME THREAD CREATION *
   ****************************************/
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

  int create_realtime_thread(RealTimeThread &thread,
                             void*(*thread_function)(void*),
                             void* args,
                             bool call_block_memory,
                             int stack_memory_factor,
                             std::vector<int> cpu_affinities){
    if(call_block_memory)
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
    ret = pthread_attr_setstacksize(&attr, stack_memory_factor*PTHREAD_STACK_MIN);
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
    param.sched_priority = 80;
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
    ret = pthread_create(&thread, &attr, thread_function, args);
    if (ret) {
      printf("%s %d\n", ("create pthread failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      return ret;
    }

    if(cpu_affinities.size()>0)
    {
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      for (unsigned i=0 ; i<cpu_affinities.size() ; ++i)
      {
        CPU_SET(cpu_affinities[i], &cpuset);
      }
      ret = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
      if (ret)
      {
        printf("%s %d\n", ("Associate thread to a specific cpu failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      }

      int get_aff_error = 0;
      get_aff_error = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
      if (get_aff_error)
      {
        printf("%s %d\n", ("Check the thread cpu affinity failed. Ret=" +
                    rt_preempt_error_message).c_str(), ret);
      }
      printf("Set returned by pthread_getaffinity_np() contained:\n");
      for (j = 0; j < CPU_SETSIZE; j++)
      {
        if (CPU_ISSET(j, &cpuset))
        {
          printf("    CPU %d\n", j);
        }
      }
    }
    return ret;
  }

  int join_thread(RealTimeThread &thread)
  {
    int ret ;
    /* Join the thread and wait until it is done */
    ret = pthread_join(thread, nullptr);
    if (ret)
      printf("join pthread failed.\n");
    return ret;
  }

  void block_memory()
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
  int create_realtime_thread(RealTimeThread &thread,
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

  void block_memory()
  {
    throw std::runtime_error("block_memory: "
                             "Please implement this for xenomai");
  }
#endif // Defined XENOMAI

  /**********************************************************
   * TODO: Check the implementation of this thread creation *
   **********************************************************/
#if defined NON_REAL_TIME
  int create_realtime_thread(RealTimeThread &thread,
                             void*(*thread_function)(void*),
                             void* args,
                             bool call_block_memory,
			                       int stack_factor,
                             std::vector<int> cpu_affinities){
    printf("Warning this thread is not going to be real time.\n");

    /* Create a pthread with specified attributes */
    thread = RealTimeThread(thread_function, args);
    return 0;
  }

  int join_thread(RealTimeThread &thread)
  {
    if(thread.joinable())
    {
      thread.join();
    }
    return 1;
  }

  void block_memory()
  {
    // do nothing
  }
#endif // Defined NON_REAL_TIME

} // namespace real_time_tools
