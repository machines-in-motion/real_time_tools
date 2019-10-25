/**
 * @file realtime_check.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief Utilities to check if the real_time capabilities of an algorithm is
 * maintained or not.
 */
#include "real_time_tools/realtime_check.hpp"
#include <fstream>

namespace real_time_tools {


  Realtime_check::Realtime_check(double target_frequency,
				 double switch_frequency ){
  
    this->target_frequency = target_frequency;
    this->started = false;
    this->ticks = 0;
    this->switchs = 0;
    this->worse_frequency = std::numeric_limits<double>::max();
    this->current_frequency = std::numeric_limits<double>::max();
    this->switch_frequency = switch_frequency;
    this->average_frequency = -1.0;
  }

  bool Realtime_check::was_realtime_lost() const {
    
    if(!this->started) {
      return false;
    }

    if(this->switchs>0){
      return true;
    }

    return false;

  }

  void Realtime_check::tick(){

    std::lock_guard<std::mutex> guard(this->mutex);
    
    std::chrono::high_resolution_clock::time_point t =
        std::chrono::high_resolution_clock::now();

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

    current_frequency = one / ( pow(10.0,-9.0) * static_cast<double>(nanos) );

    if (current_frequency < this->switch_frequency) {
      this->switchs += 1;
    }

    if (current_frequency < this->worse_frequency){
      this->worse_frequency = current_frequency;
    }

    // updating global frequency
    
    nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(t-this->start_time).count();
    
    this->average_frequency = static_cast<double>(this->ticks) / ( pow(10.0,-9.0) * static_cast<double>(nanos) );

    // preparing for next iteration

    this->last_tick = t;

  }


  double Realtime_check::get_current_frequency() const
  {
    return current_frequency;
  }
  

  bool Realtime_check::get_statistics(int &ticks,int &switchs,
				      double &target_frequency,
				      double &switch_frequency,
				      double &average_frequency,
				      double &current_frequency,
				      double &worse_frequency) {

    std::lock_guard<std::mutex> guard(this->mutex);
    
    if(!this->started){
      return false;
    }

    ticks = this->ticks;
    switchs = this->switchs;
    average_frequency = this->average_frequency;
    worse_frequency = this->worse_frequency;
    current_frequency = this->current_frequency;
    target_frequency = this->target_frequency;
    switch_frequency = this->switch_frequency;

    return true;

  }


  void print_realtime_check(Realtime_check &rc){

    int ticks,switchs;
    double average_frequency;
    double worse_frequency;
    double current_frequency;
    double target_frequency;
    double switch_frequency;
    
    bool ret = rc.get_statistics(ticks,switchs,
                                 target_frequency,
				 switch_frequency,
                                 average_frequency,
				 current_frequency,
                                 worse_frequency);

    if (!ret){
      printf("failed to get results from realtime check\n");
      return;
    }
    
    printf("nb ticks: %d\t"
           "nb switchs: %d (i.e below %f)\t"
           "target_freq: %f\t"
           "average: %f\t"
	   "current: %f\t"
           "worse: %f\n",
           ticks, switchs, switch_frequency,target_frequency,
           average_frequency,current_frequency,
	   worse_frequency);

  }
}
