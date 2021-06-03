/**
 * @file frequency_manager.hpp
 * @author Vincent Berenz (vberenz@tue.mpg.de)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2020-03-22
 *
 * @brief Tools for enforcing a desired frequency in a loop
 */

#pragma once

#include "real_time_tools/timer.hpp"

namespace real_time_tools
{
/**
 * @brief Class to have threads / loops running at a desired frequency
 */
class FrequencyManager
{
public:
    // create a manager for the desired frequency
    FrequencyManager(double frequency);

    // default constructor
    FrequencyManager();

    // Set the frequency.
    void set_frequency(double frequency);

    // Set the period in seconds.
    void set_period(double period_s);

    // predict the next sleeping time.
    double predict_sleeping_time() const;

    /**
     * @brief waits for the time such that successive calls to wait
     * will result in wait being called at the desired frequency
     * @return true if the desired frequency could be enforced
     */
    bool wait();

private:
    double period_ms_;
    double previous_time_ms_;
};
}  // namespace real_time_tools
