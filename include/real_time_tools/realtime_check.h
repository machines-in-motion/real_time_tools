#include <iostream>
#include <chrono>
#include <limits>
#include <math.h>
#include <mutex>
#include <deque>

namespace real_time_tools {

  // super simple class for checking if thread ever lost realtime. 
  // simply measure frequency between two calls to the tick function.
  class Realtime_check {

  public:

    Realtime_check(double target_frequency, double switch_frequency);

    // inform the instance of this class that an iteration passed
    void tick();

    // true if realtime was lost at least once
    // (frequency between two ticks was below target frequencies)
    bool was_realtime_lost() const;

    // return true if statistics are available, false otherwise
    // (false is returned is tick has never been called or if ticks reached 
    // maximum integer value)
    // switchs in the number of time realtime was lost.
    bool get_statistics(int &ticks,int &switchs,
                        double &target_frequency,
			double &switch_frequency,
                        double &average_frequency,
			double &current_frequency,
                        double &worse_frequency) ;






    /**
     * @brief tic measures the time when it is called. This is to be used with
     * the tac method that will return the time ellapsed between tic and tac.
     * this tic is called at least once at construction time.
     */
    void tic();

    /**
     * @brief tac is to be used after tic has been called.
     * @return the duration in seconds between the call of tic() and the call of
     * tac()
     */
    double tac();

    /**
     * @brief dump_tic_tac_measurements writes in a file the time elapsed
     * between every tick
     * @param file_name is the path to the file.
     */
    void dump_tic_tac_measurements(std::string file_name);

  private:
    
    // true if tick has been called once
    bool started;

    // time at which tick was called first
    std::chrono::high_resolution_clock::time_point start_time;

    // last time system was ticked
    std::chrono::high_resolution_clock::time_point last_tick;

    // time at which tic() was called
    std::chrono::high_resolution_clock::time_point tic_time_;

    // time at which tac() was called
    std::chrono::high_resolution_clock::time_point tac_time_;

    // frequency at which ticks are expected
    double target_frequency;

    // small quantity
    double epsilon;

    // number of iterations
    uint ticks;

    // number of time realtime was lost
    // (target frequency not respected between
    // two ticks)
    uint switchs;

    // average frequency
    double average_frequency;

    // worse frequency ever experienced
    double worse_frequency;

    // nb of switches will increase by 1
    // each time measured frequency below this
    // value
    double switch_frequency;
    
    // latest frequency that was measured
    double current_frequency;
    
    // multithreading safety
    std::mutex mutex;

    typedef std::chrono::duration<double> seconds;

    /**
     * @brief time_measurement_buffer_ this is a chained list of double
     */
    std::deque<double> time_measurement_buffer_;

    /**
     * @brief count_time_buffer_ is a counter that manages the
     * time_measurement_buffer_ fill in.
     */
    uint count_time_buffer_;

  };


  void print_realtime_check(Realtime_check &rc);


}
