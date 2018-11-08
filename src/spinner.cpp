#include "real_time_tools/spinner.hpp"
#include <iostream>
#include <pthread.h>

namespace real_time_tools {

  struct timespec next;

#ifndef MAC_OS
  void
  timespec_add_ns(struct timespec* t, long ns)
  {
    t->tv_nsec += ns;
    if (t->tv_nsec > 1000000000) {
      t->tv_nsec = t->tv_nsec - 1000000000; // + ms * 1000000;
      t->tv_sec += 1;
    }
  }
#endif

  Spinner::Spinner(double frequency) {

    // time to pass between two spins (in microseconds)
    long double time_between_spins_ld = ( (1.0 / frequency) * 1000000.0 );

#ifndef MAC_OS
    time_between_spins_ns_ = time_between_spins_ld * 1000.;
    clock_gettime(CLOCK_REALTIME, &next_);
#else
    long double zero_five = 0.5;
    long int time_between_spins_li = static_cast<long int>(time_between_spins_ld+zero_five);
    std::chrono::microseconds time_between_spins(time_between_spins_li);
    this->time_between_spins_ = time_between_spins;

    this->next_tick_ = std::chrono::system_clock::now();
#endif
  }

  void Spinner::spin() {
#ifndef MAC_OS
    timespec_add_ns(&next_, time_between_spins_ns_);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next_, NULL);
#else
    std::chrono::time_point<std::chrono::system_clock> tick = std::chrono::system_clock::now();
    std::chrono::nanoseconds diff_nanos = this->next_tick_ - tick;
    this->next_tick_ = this->next_tick_+this->time_between_spins_;
    std::chrono::microseconds diff_micros =  std::chrono::duration_cast<std::chrono::microseconds>(diff_nanos);
    long int diff = diff_micros.count();
    if(diff>0){
      usleep(diff);
    }
#endif
  }
}
