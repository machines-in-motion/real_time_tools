#include "rtpreempt_tools/realtime_thread_creation.h"

namespace rtpreempt_tools {

  int create_realtime_thread(pthread_t &thread,void*(*thread_function)(void*)){

      struct sched_param param;
      pthread_attr_t attr;
      int ret;
 
      /* Lock memory */
      if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
	printf("mlockall failed: %m\n");
	exit(-2);
      }
      
      /* Initialize pthread attributes (default values) */
      ret = pthread_attr_init(&attr);
      if (ret) {
	printf("init pthread attributes failed\n");
	goto out;
      }
      
      /* Set a specific stack size  */
      ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
      if (ret) {
	printf("pthread setstacksize failed\n");
	goto out;
      }
      
      /* Set scheduler policy and priority of pthread */
      ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
      if (ret) {
	printf("pthread setschedpolicy failed\n");
	goto out;
      }
      param.sched_priority = 80;
      ret = pthread_attr_setschedparam(&attr, &param);
      if (ret) {
	printf("pthread setschedparam failed\n");
	goto out;
      }
      /* Use scheduling parameters of attr */
      ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
      if (ret) {
	printf("pthread setinheritsched failed\n");
	goto out;
      }
      
      /* Create a pthread with specified attributes */
      ret = pthread_create(&thread, &attr, thread_function, NULL);
      if (ret) {
	printf("create pthread failed\n");
	goto out;
      }
      
      
  out:
      return ret;
      
      
  }


}
