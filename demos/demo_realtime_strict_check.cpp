/*
 * POSIX Real Time Example
 * using a single pthread as RT thread
 */

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "real_time_tools/realtime_check.h"
#include "real_time_tools/realtime_thread_creation.h"

typedef std::chrono::high_resolution_clock my_clock;

void* thread_function(void*)
{
  double freq = 1000.0; // 1kz
  int nb_iteration = 1000;

  unsigned period = static_cast<unsigned>(round((1.0/freq) * pow(10.0, 9.0)));
  my_clock::duration clock_period(period);
  real_time_tools::Realtime_check rc(freq);
  int a = 0;
  my_clock::time_point tic, tac;
  struct timespec sleep_duration, out_sleep;

  printf("reference period is %ld\n", clock_period.count());

  for(int i=0 ; i<nb_iteration ; ++i){
    rc.tick();
    tic = my_clock::now();

    a++;
    printf("sleeping time is %ld\n", sleep_duration.tv_nsec);

    tac = my_clock::now();
    sleep_duration.tv_nsec = static_cast<unsigned>(
                               0.95 * static_cast<double>(
                                 (clock_period - (tac - tic)).count()));
    nanosleep(&sleep_duration, &out_sleep); // microseconds, so in Ghz
  }

  printf("\n");
  real_time_tools::print_realtime_check(rc);
  printf("\n");

  return nullptr;
}

int main(int , char* []) {

  real_time_tools::RealTimeThread thread;
  real_time_tools::create_realtime_thread(thread, thread_function);
  real_time_tools::join_thread(thread);
}

