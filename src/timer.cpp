#include <fstream>

#include <real_time_tools/realtime_iostream.hpp>
#include <real_time_tools/timer.hpp>

namespace real_time_tools {

Timer::Timer()
{
  // initialize the tic and tac times by the current time
  tic_time_ = Timer::get_current_time_sec();
  tac_time_ = tic_time_;
  // initialize the memory buffer size
  memory_buffer_size_ = 60000;
  // default name
  name_ = "timer";
  // reserve the buffer memory
  time_measurement_buffer_.resize(memory_buffer_size_, 0.0);
  // initialize the number of time tac() is called.
  count_ = 0;
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
  // check if the buffer is full
  if (count_ >= time_measurement_buffer_.size())
  {
    time_measurement_buffer_.pop_front();
    time_measurement_buffer_.push_back(time_elapsed);
  }else{
    // save the current time elapsed
    time_measurement_buffer_[count_] = time_elapsed;
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
    std::ofstream log_file(file_name, std::ofstream::out);
    for (unsigned i=0 ; i<time_measurement_buffer_.size() ; ++i)
    {
      log_file << i << " " << time_measurement_buffer_[i] << std::endl;
    }
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

void Timer::timespec_add_sec(struct timespec& t,
                             const double duration_sec)
{
  double total_time_sec = duration_sec +
                          static_cast<double>(t.tv_nsec) / 1e9 +
                          static_cast<double>(t.tv_sec);
  total_time_sec += 0.5e-9;
  t.tv_sec = static_cast<long>(total_time_sec);
  t.tv_nsec = static_cast<long>((total_time_sec -
                                  static_cast<double>(t.tv_sec)) * 1e9);
}

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
  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &abs_target_time, NULL);
#endif
}

//void Timer::sleep_until_sec(const double& date_sec)
//{
//#ifdef MAC_OS
//  throw
//#else
//  struct timespec abs_target_time;
//  clock_gettime(CLOCK_REALTIME, &abs_target_time);
//  timespec_add_sec(&abs_target_time, sleep_duration_sec);
//  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &abs_target_time, NULL);
//#endif
//}

} // namespace real_time_tools
