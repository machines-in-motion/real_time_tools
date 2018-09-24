#include "rtpreempt_tools/realtime_thread_creation.h"

#ifdef RT_PREEMPT

namespace rtpreempt_tools {

  int create_realtime_thread_and_block_memory(pthread_t &thread,
                                              void*(*thread_function)(void*)){

    struct sched_param param;
    pthread_attr_t attr;
    int ret;

    block_memory();

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
      printf("init pthread attributes failed. Ret=%d\n", ret);
      return ret;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
      printf("pthread setstacksize failed. Ret=%d\n", ret);
      return ret;
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
      printf("pthread setschedpolicy failed. Ret=%d\n", ret);
      return ret;
    }
    param.sched_priority = 80;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
      printf("pthread setschedparam failed. Ret=%d\n", ret);
      return ret;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
      printf("pthread setinheritsched failed. Ret=%d\n", ret);
      return ret;
    }

    /* Create a pthread with specified attributes */
    ret = pthread_create(&thread, &attr, thread_function, nullptr);
    if (ret) {
      printf("create pthread failed. Ret=%d\n", ret);
      if (ret == 1) {
        printf("NOTE: This program must be executed as root to get the "
               "required realtime permissions.\n");
      }
      return ret;
    }
  }

  int create_realtime_thread(pthread_t &thread,
                             void*(*thread_function)(void*)){
    // Based on:
    // https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base

    struct sched_param param;
    pthread_attr_t attr;
    int ret;

    ret = pthread_attr_init(&attr);
    if (ret) {
      printf("init pthread attributes failed\n");
      return ret;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
      printf("pthread setstacksize failed\n");
      return ret;
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
      printf("pthread setschedpolicy failed\n");
      return ret;
    }
    param.sched_priority = 80;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
      printf("pthread setschedparam failed\n");
      return ret;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
      printf("pthread setinheritsched failed\n");
      return ret;
    }

    /* Create a pthread with specified attributes */
    ret = pthread_create(&thread, &attr, thread_function, nullptr);
    if (ret) {
      printf("create pthread failed. Ret=%d\n", ret);
      if (ret == 1) {
        printf("NOTE: This program must be executed as root to get the "
               "required realtime permissions.\n");
      }
      return ret;
    }
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

  int join_thread(pthread_t &thread)
  {
    int ret ;
    /* Join the thread and wait until it is done */
    ret = pthread_join(thread, nullptr);
    if (ret)
      printf("join pthread failed.\n");
    return ret;
  }

} // namespace rtpreempt_tools

#endif
