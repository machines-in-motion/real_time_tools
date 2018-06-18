#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>


namespace rtpreempt_tools {

  int create_realtime_thread(pthread_t &thread,void*(*thread_function)(void*));


}
