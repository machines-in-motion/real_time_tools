#include <iostream>
#include <chrono>
#include <limits>
#include <math.h>

namespace rtpreempt_tools {

  // super simple class for checking if thread ever lost realtime. 
  // simply measure frequency between two calls to the tick function.


  class Realtime_check {

  public:

    Realtime_check(float target_frequency);

    // inform the instance of this class that an iteration passed
    void tick();

    // true if realtime was lost at least once
    // (frequency between two ticks was below target frequencies)
    bool was_realtime_lost();

    // return true if statistics are available, false otherwise
    // (false is returned is tick has never been called or if ticks reached 
    // maximum integer value)
    // switchs in the number of time realtime was lost.
    bool get_statistics(int &ticks,int &switchs,
			float &average_frequency, 
			float &worse_frequency);

  private:
    
    // true if tick has been called once
    bool started;

    // time at which tick was called first
    std::chrono::high_resolution_clock::time_point start_time;

    // last time system was ticked
    std::chrono::high_resolution_clock::time_point last_tick;

    // frequency at which ticks are expected
    float target_frequency;

    // number of iterations
    uint ticks;

    // number of time realtime was lost
    // (target frequency not respected between
    // two ticks)
    uint switchs;

    // average frequency
    float average_frequency;

    // worse frequency ever experienced
    float worse_frequency;
    


  };


  void print_realtime_check(Realtime_check &rc);


}
