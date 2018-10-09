#include "real_time_tools/spin.hpp"

namespace real_time_tools {

  Spinner::Spinner(double frequency)
    : microseconds_(((1.0 / frequency) / 1000000.0)){

      this->previous_tick_ = std::chrono::system_clock::now();

  }

  void Spinner::spin(){

    std::chrono::high_resolution_clock::time_point tick = std::chrono::system_clock::now();
    std::chrono::nanoseconds time_passed_ = tick - previous_tick;
    std::chrono::microseconds time_passed = std::chrono::duration_cast<std::chrono::microseconds>(time_passed_);
    this->previous_tick_ = tick;
    
    // we fail to go as fast as expected ...
    if (time_passed>this->microseconds_){
      return;
    }

    std::chrono::microseconds diff = this->microseconds_ - time_passed;
    usleep(diff.count);

  }


}
