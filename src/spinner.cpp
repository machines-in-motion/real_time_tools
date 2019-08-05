/**
 * @file spinner.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief This file implements a spinner to time a loop
 */

#include <real_time_tools/timer.hpp>
#include <real_time_tools/spinner.hpp>
#include <iostream>
#include <pthread.h>

namespace real_time_tools {

  Spinner::Spinner() {
    period_sec_ = 0.0;
    next_date_sec_ = Timer::get_current_time_sec();
  }

  void Spinner::initialize() {
    next_date_sec_ = Timer::get_current_time_sec();
  }

  void Spinner::spin() {
    next_date_sec_ += period_sec_;
    Timer::sleep_until_sec(next_date_sec_);
  }

  double Spinner::predict_sleeping_time()
  {
    return (next_date_sec_ + period_sec_) - Timer::get_current_time_sec();
  }
}
