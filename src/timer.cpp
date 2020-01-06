/**
 * @file timer.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief This file implements tools to acquire the time, the date,
 * and do timing measurement 
 */

#include <fstream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <real_time_tools/iostream.hpp>
#include <real_time_tools/timer.hpp>

namespace real_time_tools {

/**
 * @brief Simple renaming to get the number of days passed out of the date.
 */
typedef std::chrono::duration<
  int, std::ratio_multiply<std::chrono::hours::period,
  std::ratio<24> >::type> days;

std::string Timer::get_current_date_str()
{
  std::ostringstream oss;
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  struct tm *parts = std::localtime(&now_c);
  oss << (int)(1900 + parts->tm_year) << "-";
  oss << std::setfill('0') << std::setw(2) << 1 + parts->tm_mon << "-";
  oss << std::setfill('0') << std::setw(2) << parts->tm_mday << "_";
  oss << std::setfill('0') << std::setw(2) << parts->tm_hour << "-";
  oss << std::setfill('0') << std::setw(2) << parts->tm_min << "-";
  oss << std::setfill('0') << std::setw(2) << parts->tm_sec;
  return oss.str();
}

Timer::Timer()
{
  // initialize the tic and tac times by the current time
  tic_time_ = std::numeric_limits<double>::quiet_NaN();
  // initialize the memory buffer size, allocate memory and set counter to zero.
  set_memory_size(60000);
  // default name
  name_ = "timer";
  // reset all the statistic memebers
  min_elapsed_time_ = std::numeric_limits<double>::infinity();
  max_elapsed_time_ = -std::numeric_limits<double>::infinity();
  avg_elapsed_time_ = 0.0;
  second_moment_elapsed_time_ = 0.0;
  count_ = 0;
}

void Timer::tic()
{
  // get the current time
  tic_time_ = Timer::get_current_time_sec();
}

double Timer::tac()
{
  double tac_time = Timer::get_current_time_sec();
  double time_interval = tac_time - tic_time_;

  log_time_interval(time_interval);

  return time_interval;
}

double Timer::tac_tic()
{
    double tac_time = Timer::get_current_time_sec();
    double time_interval = tac_time - tic_time_;

    log_time_interval(time_interval);

    tic_time_ = tac_time;

    return time_interval;
}


void Timer::log_time_interval(double time_interval)
{
    if(std::isnan(time_interval))
        return;

    // Only store into the buffer if the buffer is non-zero.
    if (memory_buffer_size_ != 0) {
      // check if the buffer is full
      if (count_ >= time_measurement_buffer_.size())
      {
        time_measurement_buffer_.pop_front();
        time_measurement_buffer_.push_back(time_interval);
      } else {
        // save the current time elapsed
        time_measurement_buffer_[count_] = time_interval;
      }
    }

    // increase the count
    ++count_;
    // compute some statistics
    min_elapsed_time_ = time_interval < min_elapsed_time_ ?
                           time_interval : min_elapsed_time_;
    max_elapsed_time_ = time_interval > max_elapsed_time_ ?
                          time_interval : max_elapsed_time_;
    avg_elapsed_time_ = (double(count_ - 1) * avg_elapsed_time_ + time_interval) /
                        double(count_);
    second_moment_elapsed_time_ =
        (double(count_ - 1) * second_moment_elapsed_time_ +
         time_interval * time_interval) / double(count_);
}



void Timer::dump_measurements(std::string file_name) const
{
  try{
    std::ofstream log_file(file_name, std::ios::binary | std::ios::out);
    log_file.precision(10);
    for (unsigned i = 0;
            i < std::min(count_, (unsigned long)memory_buffer_size_); ++i)
    {
      log_file << i << " " << time_measurement_buffer_[i] << std::endl;
    }
    log_file.flush();
    log_file.close();
  }catch(...){
    rt_printf("fstream Error in dump_tic_tac_measurements(): "
              "no time measurment saved\n");
  }
}

void Timer::print_statistics() const
{
  rt_printf("%s --------------------------------\n", name_.c_str());
  rt_printf("count: %ld\n"
            "min_elapsed_sec: %f\n"
            "max_elapsed_sec: %f\n"
            "avg_elapsed_sec: %f\n"
            "std_dev_elapsed_sec: %f\n",
            count_,
            get_min_elapsed_sec(),
            get_max_elapsed_sec(),
            get_avg_elapsed_sec(),
            get_std_dev_elapsed_sec());
  rt_printf("--------------------------------------------\n");
}

#ifndef MAC_OS
void Timer::timespec_add_sec(struct timespec& date_spec,
                             const double duration_sec)
{
  double total_time_sec = duration_sec +
                          static_cast<double>(date_spec.tv_nsec) / 1e9 +
                          static_cast<double>(date_spec.tv_sec);
  sec_to_timespec(total_time_sec, date_spec);
}

void Timer::sec_to_timespec(double date_sec, struct timespec& date_spec)
{
  date_sec += 0.5e-9;
  date_spec.tv_sec = static_cast<long>(date_sec);
  date_spec.tv_nsec =
      static_cast<long>((date_sec -
                         static_cast<double>(date_spec.tv_sec)) * 1e9);
}

#endif

double Timer::get_current_time_sec()
{
#ifdef MAC_OS
  throw
  return std::nan();
#else
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return static_cast<double>(now.tv_sec) +
      static_cast<double>(now.tv_nsec) / 1e9;
#endif
}


int Timer::sleep_microseconds(int sleep_duration_us)
{
  #ifdef MAC_OS
  throw
  #elseif XENOMAI
  return rt_task_sleep(sleep_duration_us*1e3);
  #else
  usleep(sleep_duration_us);
  return 0;
  #endif
}
  
void Timer::sleep_sec(const double& sleep_duration_sec)
{
#ifdef MAC_OS
  throw
#else
  struct timespec abs_target_time;
  clock_gettime(CLOCK_REALTIME, &abs_target_time);
  timespec_add_sec(abs_target_time, sleep_duration_sec);
  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &abs_target_time, nullptr);
#endif
}

void Timer::sleep_until_sec(const double& date_sec)
{
#ifdef MAC_OS
  throw
#else
  struct timespec abs_target_time;
  sec_to_timespec(date_sec, abs_target_time);
  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &abs_target_time, nullptr);
#endif
}

} // namespace real_time_tools
