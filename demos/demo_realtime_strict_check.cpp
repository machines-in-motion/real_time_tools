/**
 * @file demo_realtime_strict_check.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief Check the real time capbilites of a loop.
 */

#include "real_time_tools/realtime_check.hpp"
#include "real_time_tools/thread.hpp"

/** @brief define an alias for the clock */
typedef std::chrono::high_resolution_clock my_clock;

/** @brief this function is executed in a real_time_thread. */
THREAD_FUNCTION_RETURN_TYPE thread_function(void*)
{
  double freq = 1000.0; // 1kz
  double switch_freq = 990;
  int nb_iteration = 1000;

  unsigned period = static_cast<unsigned>(round((1.0/freq) * pow(10.0, 9.0)));
  my_clock::duration clock_period(period);
  real_time_tools::RealTimeCheck rc(freq,switch_freq);
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
  rc.print();
  printf("\n");

  return THREAD_FUNCTION_RETURN_VALUE;
}

/** @brief This demos show the used of the strict check of the real time loop.*/
int main(int , char* []) {
  real_time_tools::RealTimeThread thread;
  thread.create_realtime_thread(thread_function);
  thread.join();
}

/**
 * \example demo_realtime_strict_check.cpp
 * 
 * This demos has for purpose to present the class
 * real_time_tools::RealTimeCheck. This class
 * measures the frequency of a loop and compares it with a threshold frequency.
 * As demonstrated below, the class takes as input the desired frequency and
 * the threshold frequency.
 * 
 * In order to enable the measurement of the your loop one need to call the
 * real_time_tools::RealTimeCheck::tick() function.
 * 
 * Finally the statistical results can be displayed via the
 * real_time_tools::RealTimeCheck::print() methods.
 * 
 * The difference with the demo_realtime_check.cpp
 * is that we measure the sleeping time as well.
 */
