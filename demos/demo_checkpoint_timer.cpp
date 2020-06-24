/**
 * @file
 * @license BSD 3-clause
 * @copyright Copyright (c) 2020, New York University and Max Planck
 *            Gesellschaft
 */
/**
 * @example demo_checkpoint_timer.cpp
 * @brief Demo on how to use the CheckpointTimer.
 *
 * Note that when the statistics are printed like this, the printing is included
 * in the time measurement of the total loop duration.
 */

#include <real_time_tools/checkpoint_timer.hpp>
#include <real_time_tools/timer.hpp>

//! @brief Dummy function
void init()
{
    real_time_tools::Timer::sleep_ms(3);
}
//! @brief Dummy function
void do_some_stuff()
{
    real_time_tools::Timer::sleep_ms(20);
}
//! @brief Dummy function
void write_log()
{
    real_time_tools::Timer::sleep_ms(6);
}

//! @brief Simple example on how to use the CheckpointTimer in a loop.
int main()
{
    //! [Usage of CheckpointTimer]

    // set second template argument to false to disable timer
    real_time_tools::CheckpointTimer<3, true> timer;

    for (int i = 0; i < 1000; i++)
    {
        timer.start();

        init();
        timer.checkpoint("initialize");

        do_some_stuff();
        timer.checkpoint("do some stuff");

        write_log();
        timer.checkpoint("logging");

        // print the timing results every 100 iterations
        if (i % 100 == 0 && i > 0)
        {
            timer.print_statistics();
        }
    }

    //! [Usage of CheckpointTimer]
    return 0;
}
