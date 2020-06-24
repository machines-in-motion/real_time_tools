/**
 * @file timer.hpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Some tools to measure (ellapsed) time.
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include <unistd.h>
#include <chrono>
#include <cmath>
#include <deque>
#include <string>

#include "real_time_tools/iostream.hpp"

namespace real_time_tools
{
/**
 * @brief The timer class is a simple time measurement class that measure
 * between tic and tac and with a memory buffer of a certain size.
 */
class Timer
{
public:
    /**
     * @brief timer constructor
     */
    Timer();

    /**
     * @brief tic measures the time when it is called. This is to be used with
     * the tac method that will return the time elapsed between tic and tac.
     */
    void tic();

    /**
     * @brief tac is to be used after tic has been called.
     * @return the duration in seconds between the call of tic() and the call of
     * tac(). if tic() has not been called previously this will return nan
     */
    double tac();

    /**
     * @brief this is like a tac() followed by a tic(), making sure the
     * previous tac_time becomes the tic_time
     */
    double tac_tic();

    /**
     * @brief Save the time interval measured
     *
     * @param time_interval
     */
    void log_time_interval(double time_interval);

    /**
     * IOSTREAM functions
     */

    /**
     * @brief dump_tic_tac_measurements writes in a file the time elapsed
     * between every tick
     * @param file_name is the path to the file.
     */
    void dump_measurements(std::string file_name) const;

    /**
     * @brief print_statistics display in real time the statistics of the time
     * measurements acquiered so far.
     */
    void print_statistics() const;

    /**
     * SETTERS
     */

    /**
     * @brief set_memory_size sets the buffer size. It resets all value of the
     * buffer to zero.
     * !! WARNING non real time method. !!
     * @param memory_buffer_size is the size use to reset the size of the
     */
    void set_memory_size(const unsigned memory_buffer_size)
    {
        count_ = 0;
        memory_buffer_size_ = memory_buffer_size;
        time_measurement_buffer_.resize(memory_buffer_size_, 0.0);
    }

    /**
     * @brief set_name modify the name of the object for display purposes.
     * @param name is the new name of the object.
     */
    void set_name(std::string name)
    {
        name_ = name;
    }

    /**
     * GETTERS
     */

    /**
     * @brief get_min_elapsed_sec
     * @return a copy of the minimum elapsed times
     */
    double get_min_elapsed_sec() const
    {
        return min_elapsed_time_;
    }

    /**
     * @brief get_max_elapsed_sec
     * @return a copy of the maximum elapsed times
     */
    double get_max_elapsed_sec() const
    {
        return max_elapsed_time_;
    }

    /**
     * @brief get_avg_elapsed_sec
     * @return a copy of the average elapsed time
     */
    double get_avg_elapsed_sec() const
    {
        return avg_elapsed_time_;
    }

    /**
     * @brief get_std_dev_elapsed_sec
     * @return a copy of the standard deviation of the elapsed times
     */
    double get_std_dev_elapsed_sec() const
    {
        return std::sqrt(second_moment_elapsed_time_ -
                         avg_elapsed_time_ * avg_elapsed_time_);
    }

protected:
    /**
     * @brief tic_time_ time at which tic() was called
     */
    double tic_time_;

    /**
     * @brief time_measurement_buffer_ this is a chained list of double
     */
    std::deque<double> time_measurement_buffer_;

    /**
     * @brief count_time_buffer_ is a counter that manages the
     * time_measurement_buffer_ fill in.
     */
    long unsigned count_;

    /**
     * @brief memory_buffer_size_ is the max size of the memory buffer.
     */
    unsigned memory_buffer_size_;

    /**
     * @brief min_elapsed_time_ is the minimum measured elapsed time
     */
    double min_elapsed_time_;

    /**
     * @brief max_elapsed_time_ is the maximum measured elapsed time
     */
    double max_elapsed_time_;

    /**
     * @brief avg_elapsed_time_ is the average measured elapsed time
     */
    double avg_elapsed_time_;

    /**
     * @brief avg_elapsed_time_ is the second moment measured elapsed
     * time
     */
    double second_moment_elapsed_time_;

    /**
     * @brief name_ of the timer object
     */
    std::string name_;

    /**
     * Some utilities
     */
public:
    /**
     * @brief get_current_time_sec gives the current time in double and in
     * seconds
     * @return
     */
    static double get_current_time_sec();

    /**
     * @brief get_current_time_ms gives the current time in double and in
     * milli seconds
     * @return
     */
    static double get_current_time_ms()
    {
        return 1e3 * get_current_time_sec();
    }

    /**
     * @brief puts the current thread to sleep for the duration
     * of "sleep_duration_us" micro-seconds.
     * @param sleep_time_us is the sleeping duration asked in micro-seconds.
     * @return 0 on success, error code otherwise
     */
    static int sleep_microseconds(int sleep_duration_us);

    /**
     * @brief sleep_sec puts the current thread to sleep for the duration
     * of "sleep_time_sec" seconds.
     * @param sleep_time_sec is the sleeping duration asked in seconds.
     */
    static void sleep_sec(const double& sleep_time_sec);

    /**
     * @brief sleep_ms puts the current thread to sleep for the duration
     * of "sleep_time_sec" seconds.
     * @param sleep_time_ms is the sleeping duration asked in seconds.
     */
    static void sleep_ms(const double& sleep_time_ms)
    {
        sleep_sec(1e-3 * sleep_time_ms);
    }

    /**
     * @brief sleep_until_sec puts the threads to sleep until the date
     * "date_sec" is reached.
     * @param date_sec is the date until when to sleep in seconds.
     */
    static void sleep_until_sec(const double& date_sec);

#ifndef MAC_OS
    /**
     * @brief timespec_add_sec posix type of a date in time.
     * @param date_spec is the date to be changed
     * @param duration_sec the duration to be added to "t" in seconds
     */
    static void timespec_add_sec(struct timespec& date_spec,
                                 const double duration_sec);

    /**
     * @brief sec_to_timespec converts a double representing the time in seconds
     * to a struct timespec.
     * @param[in] date_sec is the time in sec to be converted.
     * @param[out] date_spec is the converted structure.
     */
    static void sec_to_timespec(double date_sec, struct timespec& date_spec);

#endif  // MAC_OS

    /**
     * @brief get_current_date_str get the current date and format it in a
     * string with "year_month_day_hour_minute_sec"
     */
    static std::string get_current_date_str();
};

}  // namespace real_time_tools

#endif  // TIMER_HPP
