#include <fstream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <real_time_tools/realtime_iostream.hpp>
#include <real_time_tools/timer.hpp>

namespace real_time_tools {


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
  tic_time_ = Timer::get_current_time_sec();
  tac_time_ = tic_time_;
  // initialize the memory buffer size, allocate memory and set counter to zero.
  set_memory_size(60000);
  // default name
  name_ = "timer";
  // reset all the statistic memebers
  min_elapsed_time_ = std::numeric_limits<double>::infinity();
  max_elapsed_time_ = -std::numeric_limits<double>::infinity();
  avg_elapsed_time_ = 0.0;
  second_moment_elapsed_time_ = 0.0;
}

void Timer::tic()
{
  // get the current time
  tic_time_ = Timer::get_current_time_sec();
}

double Timer::tac()
{
  // get the current time
  tac_time_ = Timer::get_current_time_sec();
  // getting the time elapsed
  double time_elapsed = tac_time_ - tic_time_;

  // Only store into the buffer if the buffer is non-zero.
  if (memory_buffer_size_ != 0) {
    // check if the buffer is full
    if (count_ >= time_measurement_buffer_.size())
    {
      time_measurement_buffer_.pop_front();
      time_measurement_buffer_.push_back(time_elapsed);
    } else {
      // save the current time elapsed
      time_measurement_buffer_[count_] = time_elapsed;
    }
  }

  // increase the count
  ++count_;
  // compute some statistics
  min_elapsed_time_ = time_elapsed < min_elapsed_time_ ?
                         time_elapsed : min_elapsed_time_;
  max_elapsed_time_ = time_elapsed > max_elapsed_time_ ?
                        time_elapsed : max_elapsed_time_;
  avg_elapsed_time_ = (double(count_ - 1) * avg_elapsed_time_ + time_elapsed) /
                      double(count_);
  second_moment_elapsed_time_ =
      (double(count_ - 1) * second_moment_elapsed_time_ +
       time_elapsed * time_elapsed) / double(count_);
  return time_elapsed;
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
