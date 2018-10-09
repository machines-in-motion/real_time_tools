#pragma once

#include <chrono>
#include <unistd.h>

namespace real_time_tools {

  class Spinner {

  public :

    Spinner(double frequency);

    void spin();

  private:

    double frequency_;
    std::chrono::high_resolution_clock::time_point next_tick_; 
    std::chrono::microseconds time_between_spins_;

  };

}

