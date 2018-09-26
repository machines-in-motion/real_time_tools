/*
 * POSIX Real Time Example
 * using a single pthread as RT thread
 */

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "real_time_tools/realtime_check.h"
#include "real_time_tools/realtime_thread_creation.h"

void *thread_function(void *data)
{

  real_time_tools::Realtime_check rc(1000.0);

  int a = 0;
  for(int i=0;i<1000;i++){
    rc.tick();
    a++;
    usleep(2000); // microseconds, so 1Ghz
  }

  printf("\n");
  real_time_tools::print_realtime_check(rc);
  printf("\n");

  return nullptr;
}

int main(int argc, char* argv[]) {

  pthread_t thread;
  real_time_tools::create_realtime_thread(thread, thread_function);
  real_time_tools::join_thread(thread);
}

