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
    std::chrono::high_resolution_clock::time_point next_tick_; 
    std::chrono::microseconds time_between_spins_;

  };

}

