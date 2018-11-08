#pragma once

#include <chrono>
#include <unistd.h>

namespace real_time_tools {

  // class to have threads / loops running at a desired frequency
  class Spinner {

  public :

    // create a spinner for the desired frequency
    Spinner(double frequency);

    // wait for the time such that successive calls to spin
    // will result in spin being called at the desired frequency
    void spin();

  private:

    double frequency_;

#ifndef MAC_OS
    struct timespec next_;
    long time_between_spins_ns_;
#else
    std::chrono::time_point<std::chrono::system_clock> next_tick_;
    std::chrono::microseconds time_between_spins_;
#endif
  };

}
