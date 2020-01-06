/**
 * @file demo_realtime_check.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief Check the real time capbilites of a loop.
 */
#include "real_time_tools/timer.hpp"
#include "real_time_tools/realtime_check.hpp"
#include "real_time_tools/thread.hpp"

/*! Real time thread that measure the spinning frequency and do some basic
 *  operation */
THREAD_FUNCTION_RETURN_TYPE thread_function(void*)
{
  double freq = 1000.0; // 1kz
  double switch_freq = 990;
  real_time_tools::RealTimeCheck rc(freq,switch_freq);
  int nb_iteration = 10000;
  int a = 0;

  printf("sleeping time is %f seconds", 1.0/freq);

  for(int i=0 ; i<nb_iteration ; ++i){
    rc.tick();
    a++;
    real_time_tools::Timer::sleep_sec(1.0/freq); // microseconds, so in Ghz
  }

  printf("\n");
  rc.print();
  printf("\n");

  return THREAD_FUNCTION_RETURN_VALUE;
}

/*! Create a real time thread and measure the frequency of the thread. */
int main(int , char* []) {
  real_time_tools::RealTimeThread thread;
  thread.create_realtime_thread(thread_function);
  thread.join();
}

/**
 * \example demo_realtime_check.cpp
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
 */
