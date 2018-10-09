#include "real_time_tools/spinner.h"
#include <iostream>

namespace real_time_tools {

  Spinner::Spinner(double frequency) {

    // time to pass between two spins (in microseconds)
    long double time_between_spins_ld = ( (1.0 / frequency) * 1000000.0 );
    long double zero_five = 0.5;
    long int time_between_spins_li = static_cast<long int>(time_between_spins_ld+zero_five);
    std::chrono::microseconds time_between_spins(time_between_spins_li);
    this->time_between_spins_ = time_between_spins;
    
    this->next_tick_ = std::chrono::system_clock::now();

  }

  void Spinner::spin(){

    std::chrono::high_resolution_clock::time_point tick = std::chrono::system_clock::now();
    std::chrono::nanoseconds diff_nanos = this->next_tick_ - tick;
    this->next_tick_ = this->next_tick_+this->time_between_spins_;
    std::chrono::microseconds diff_micros =  std::chrono::duration_cast<std::chrono::microseconds>(diff_nanos);
    long int diff = diff_micros.count();
    if(diff>0){
      usleep(diff);
    }
    
  }


}
