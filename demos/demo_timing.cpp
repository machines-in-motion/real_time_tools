/**
 * @file demo_timing.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief Demo of the Timer class usage.
 */

#include "real_time_tools/spinner.hpp"
#include "real_time_tools/thread.hpp"
#include "real_time_tools/realtime_check.hpp"
#include "real_time_tools/timer.hpp"

/** @brief Real time thread presenting the use of the Timer class. */
void thread_function(void*)
{

    double frequency = 1000;

    real_time_tools::Spinner spinner;
    spinner.set_frequency(frequency);

    real_time_tools::Timer timer;

    while(true)
    {
        for(int i = 0; i < frequency; i++)
        {
            spinner.spin();
            timer.tac_tic();
        }

        timer.print_statistics();
    }

    //return NULL;
}

/** @brief Launch a real time thread presenting the use of the Timer class. */
int main(int , char* []) {
  real_time_tools::RealTimeThread thread;
  thread.create_realtime_thread(thread_function);
  thread.join();
}

/**
 * \example demo_timing.cpp
 * 
 * This demos has for purpose to present the class real_time_tools::Timer.
 * This class allows you to use the real time clocks. And measure durations
 * and extract statistics on them.
 * 
 * Inn this example we create a simple loop cadence by the
 * real_time_tools::Spinner. And we measure the period of the loop.
 * 
 * In order to do so one need to create a real_time_tools::Timer and call the
 * real_time_tools::Timer::tac_tic() method which compute the duration between
 * each call of this method.
 * 
 * The demo displays the statistics of the measured time every milliseconds.
 */
