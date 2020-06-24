/**
 * @file demo_thread.cpp
 * @author Vincent Berenz
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2020-01-02
 *
 * @brief Mininal thread example
 */

#include "real_time_tools/thread.hpp"
#include "real_time_tools/timer.hpp"

THREAD_FUNCTION_RETURN_TYPE thread_function(void*)
{
    int i = 0;
    while (true)
    {
        i += 1;
        if (i % 1000 == 0)
        {
            rt_printf("iteration %d\n", i);
        }
        real_time_tools::Timer::sleep_microseconds(1000);
    }
    return THREAD_FUNCTION_RETURN_VALUE;
}

int main()
{
    real_time_tools::RealTimeThread thread;
    thread.block_memory();
    thread.create_realtime_thread(thread_function);
    thread.join();
}
