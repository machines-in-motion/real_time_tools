#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <string>
#include <deque>
#include <cmath>

#include "real_time_tools/realtime_iostream.hpp"

namespace real_time_tools {

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
     * this tic is called at least once at construction time.
     */
    void tic();

    /**
     * @brief tac is to be used after tic has been called.
     * @return the duration in seconds between the call of tic() and the call of
     * tac()
     */
    double tac();

    /**
     * @brief get_current_time_sec give the current time in double and in
     * seconds
     * @return
     */
    static double get_current_time_sec();

    /**
     * @brief sleep_sec
     * @param sleep_time_sec
     */
    static void sleep_sec(const double& sleep_time_sec);

#ifndef MAC_OS
    /**
     * @brief timespec_add_sec posix type of a date in time.
     * @param t is the date to be changed
     * @param duration_sec the duration to be added to "t" in seconds
     */
    static void timespec_add_sec(struct timespec& t, const double duration_sec);
#endif // MAC_OS


    /**
      * UTILITIES
      * /

    /**
     * @brief dump_tic_tac_measurements writes in a file the time elapsed
     * between every tick
     * @param file_name is the path to the file.
     */
    void dump_measurements(std::string file_name)  const;

    /**
     * @brief print_statistics
     */
    void print_statistics() const;

    /**
      * SETTERS
      * /

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
     * @brief set_name
     * @param name
     */
    void set_name(std::string name)
    {
      name_ = name;
    }

    /**
      * GETTERS
      * /

    /**
     * @brief get_min_elapsed_sec
     * @return
     */
    double get_min_elapsed_sec() const
    {
      return min_elapsed_time_;
    }

    double get_max_elapsed_sec() const
    {
      return max_elapsed_time_;
    }

    double get_avg_elapsed_sec() const
    {
      return avg_elapsed_time_;
    }

    double get_std_dev_elapsed_sec() const
    {
      return std::sqrt(second_moment_elapsed_time_ -
                       avg_elapsed_time_ * avg_elapsed_time_);
    }

  private:

    /**
     * @brief tic_time_ time at which tic() was called
     */
    double tic_time_;

    /**
     * @brief tac_time_  time at which tac() was called
     */
    double tac_time_;

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

  };

} // namespace real_time_tools


#endif // TIMER_HPP
