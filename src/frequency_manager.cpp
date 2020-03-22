#include "real_time_tools/frequency_manager.hpp"

#include <iostream>

namespace real_time_tools
{
  
  FrequencyManager::FrequencyManager(double frequency)
    : period_ms_((1.0/frequency)*1000.0),
      previous_time_ms_(-1) {}
      

  bool FrequencyManager::wait()
  {
    double t = Timer::get_current_time_ms();
    if (previous_time_ms_<0)
      {
	previous_time_ms_ = t;
	return true;
      }
    double delta = t - previous_time_ms_;
    if (delta > period_ms_)
      {
	previous_time_ms_ = t;
	return false;
      }
    Timer::sleep_ms(period_ms_-delta);
    previous_time_ms_ = Timer::get_current_time_ms();
    return true;
  }

}
