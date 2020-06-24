/**
 * @file realtime_check.hpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Tools for checking the real time of an algorithm.
 */

#ifndef REALTIME_CHECK_HPP
#define REALTIME_CHECK_HPP

#include <math.h>
#include <chrono>
#include <iostream>
#include <limits>
#include <mutex>

namespace real_time_tools
{
/**
 * @brief super simple class for checking if thread ever lost realtime.
 * simply measure frequency between two calls to the tick function.
 *
 */
class RealTimeCheck
{
public:
    /**
     * @brief Construct a new RealTimeCheck object.
     *
     * @param target_frequency is the loop frequency.
     * @param switch_frequency is the admissible frequency.
     */
    RealTimeCheck(double target_frequency, double switch_frequency);

    /*! inform the instance of this class that an iteration passed */
    void tick();

    /*! true if realtime was lost at least once
     * (frequency between two ticks was below target frequencies) */
    bool was_realtime_lost() const;

    /*! return true if statistics are available, false otherwise
     *  (false is returned is tick has never been called or if ticks reached
     *  maximum integer value)
     *  switchs in the number of time realtime was lost.
     */
    bool get_statistics(int &ticks,
                        int &switchs,
                        double &target_frequency,
                        double &switch_frequency,
                        double &average_frequency,
                        double &current_frequency,
                        double &worse_frequency);

    /*! return the averaged observed frequency if statistics are available, -1
     * otherwise (false is returned is tick has never been called or if ticks
     * reached maximum integer value).
     */
    double get_average_frequency();

    /*! returns observed frequency after last call to tick */
    double get_current_frequency() const;

    /*! Display the results of the frequency measurement. */
    void print();

private:
    /*! true if tick has been called once */
    bool started;

    /*! time at which tick was called first*/
    std::chrono::high_resolution_clock::time_point start_time;

    /*! last time system was ticked */
    std::chrono::high_resolution_clock::time_point last_tick;

    /*! frequency at which ticks are expected */
    double target_frequency;

    /*! small quantity */
    double epsilon;

    /*! number of iterations */
    uint ticks;

    /*! number of time realtime was lost (target frequency not respected between
     * two ticks) */
    uint switchs;

    /*! average frequency */
    double average_frequency;

    /*! worse frequency ever experienced */
    double worse_frequency;

    /*! nb of switches will increase by 1
     * each time measured frequency below this
     * value */
    double switch_frequency;

    /*! latest frequency that was measured */
    double current_frequency;

    /*! multithreading safety */
    std::mutex mutex;
};

}  // namespace real_time_tools

#endif  // REALTIME_CHECK_HPP
