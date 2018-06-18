#include "rtpreempt_tools/realtime_check.h"

namespace rtpreempt_tools {


  Realtime_check::Realtime_check(float target_frequency){
  
    this->target_frequency = target_frequency;
    this->started = false;
    this->ticks = 0;
    this->witchs = 0;
    this->worse_frequency = std::numeric_limits<float>::max();

  }

  Realtime_check::was_realtime_lost(){
    
    if(!this->started) {
      return false;
    }

    if(this->switchs>0){
      return true;
    }

    return false;

  }


  Realtime_check::tick(){
    
    std::chrono::time_point t = std::chrono::system_clock::now();
    this->ticks +=1;

    if(!this->started){
      
      this->start_time = t;
      this->last_tick = t;
      return;

    }

    this->started = true;

    // checking if current frequency (as of previous tick) is fine

    auto nanos= std::chrono::duration_cast<std::chrono::nanoseconds>(t-this->last_tick).count();
    
    double one = 1.0;

    float current_frequency = one / static_cast<float>(nanos);

    if (current_frequency<this->target_frequency) {
      this->switchs += 1;
    }

    if (current_frequency<this->worse_frequency){
      this->worse_frequency = current_frequency;
    }

    // updating global frequency
    
    nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(t-this->start_time).count();
    
    this->average_frequency = one / static_cast<float>(nanos);

    // preparing for next iteration

    this->last_tick = t;

  }

  



}
