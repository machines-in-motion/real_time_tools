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
  my_clock::time_point start, stop, mid;
  my_clock::duration sleep_duration_diff;
  struct timespec sleep_duration, out_sleep;

  printf("reference period is %ld\n", clock_period.count());

  for(int i=0 ; i<nb_iteration ; ++i){
    start = my_clock::now();

    rc.tick();


    a++;
    //printf("%d %d", sleep_duration.tv_nsec, out_sleep.tv_nsec);
    //printf("%ld ; %ld ; ", sleep_duration.tv_nsec, sleep_duration_diff.count());
    //printf("sleeping time is %ld  \n", sleep_duration.tv_nsec);

    mid = my_clock::now();
    sleep_duration.tv_nsec = (clock_period -
                              sleep_duration_diff -
                              (mid - start)).count();

    nanosleep(&sleep_duration, &out_sleep); // microseconds, so in Ghz

    stop = my_clock::now();
    sleep_duration_diff = my_clock::duration(
          (unsigned) ((stop - mid) - my_clock::duration(
                        sleep_duration.tv_nsec)).count());
  }

  printf("\n");
  real_time_tools::print_realtime_check(rc);
  printf("\n");

  return nullptr;
}

int main(int , char* []) {

  real_time_tools::RealTimeThread thread;
  real_time_tools::block_memory();
  real_time_tools::create_realtime_thread(thread, thread_function);
  real_time_tools::join_thread(thread);
}

