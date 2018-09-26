#include "real_time_tools/realtime_check.h"

namespace real_time_tools {


  Realtime_check::Realtime_check(float target_frequency){
  
    this->target_frequency = target_frequency;
    this->started = false;
    this->ticks = 0;
    this->switchs = 0;
    this->worse_frequency = std::numeric_limits<float>::max();

  }

  bool Realtime_check::was_realtime_lost(){
    
    if(!this->started) {
      return false;
    }

    if(this->switchs>0){
      return true;
    }

    return false;

  }


  void Realtime_check::tick(){
    
    std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
    this->ticks +=1;

    if(this->ticks>=std::numeric_limits<uint>::max()-1){
      this->started = false;
      return;
    }

    if(!this->started){
      this->start_time = t;
      this->last_tick = t;
      this->started = true;
      return;
    }

    // checking if current frequency (as of previous tick) is fine

    auto nanos= std::chrono::duration_cast<std::chrono::nanoseconds>(t-this->last_tick).count();
    
    double one = 1.0;

    float current_frequency = one / ( pow(10.0,-9.0) * static_cast<float>(nanos) );

    if (current_frequency<this->target_frequency) {
      this->switchs += 1;
    }

    if (current_frequency<this->worse_frequency){
      this->worse_frequency = current_frequency;
    }

    // updating global frequency
    
    nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(t-this->start_time).count();
    
    this->average_frequency = static_cast<float>(this->ticks) / ( pow(10.0,-9.0) * static_cast<float>(nanos) );

    // preparing for next iteration

    this->last_tick = t;

  }



  bool Realtime_check::get_statistics(int &ticks,int &switchs,
				      float &average_frequency, 
				      float &worse_frequency){

    if(!this->started){
      return false;
    }

    ticks = this->ticks;
    switchs = this->switchs;
    average_frequency = this->average_frequency;
    worse_frequency = this->worse_frequency;

    return true;

  }


  void print_realtime_check(Realtime_check &rc){

    int ticks,switchs;
    float average_frequency;
    float worse_frequency;

    bool ret = rc.get_statistics(ticks,switchs,
				 average_frequency, 
				 worse_frequency);

    if (!ret){
      printf("failed to get results from realtime check\n");
      return;
    }
    
    printf("nb ticks: %d\tnb switchs: %d\taverage: %f\t worse: %f\n",
	   ticks,switchs,average_frequency,worse_frequency);

  }


}
