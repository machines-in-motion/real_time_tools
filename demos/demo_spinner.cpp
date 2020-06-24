/**
 * @file demo_spinner.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Demo of the spinner class usage
 */

#include "real_time_tools/realtime_check.hpp"
#include "real_time_tools/spinner.hpp"
#include "real_time_tools/thread.hpp"

/** @brief implement a real time thread checking the timing of the loop */
THREAD_FUNCTION_RETURN_TYPE thread_function(void*)
{
    double frequency = 300.0;
    double switch_frequency = 290;

    real_time_tools::RealTimeCheck realtime_check(frequency, switch_frequency);
    real_time_tools::Spinner spinner;
    spinner.set_frequency(frequency);

    for (int i = 0; i < 500; i++)
    {
        realtime_check.tick();
        spinner.spin();
    }

    std::cout << "\n";
    realtime_check.print();
    std::cout << "\n";

    return THREAD_FUNCTION_RETURN_VALUE;
}

/** @brief This a demo on how to use the RealTimeCheck class. */
int main(int, char* [])
{
    real_time_tools::RealTimeThread thread;
    thread.create_realtime_thread(thread_function);
    thread.join();
}

/**
 * \example demo_spinner.cpp
 *
 * This demos has for purpose to present the class real_time_tools::Spinner.
 * This class
 * allows you to time a loop with a simple API.
 *
 * One need to create a spinner and set the current spinning frequency. Two
 * method are available for this: real_time_tools::Spinner::set_frequency() or
 * real_time_tools::Spinner::set_period()..
 *
 * Once this is set one just needs to call real_time_tools::Spinner::spin() and
 * the thread will
 * sleep just the amount of time needed in order for the loop to cadenced
 * properly.
 */
