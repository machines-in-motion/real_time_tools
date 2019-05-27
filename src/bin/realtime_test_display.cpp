/**
 * @file realtime_test_display.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Display results of the real time test
 */

#include "real_time_tools/realtime_test.hpp"
#include "shared_memory/shared_memory.hpp"
#include "real_time_tools/spinner.hpp"
#include <signal.h>


static bool running;


void stop(int){
  running=false;
}


int main(int nb_args, char** argv) {

  // exit on ctrl+c
  struct sigaction stopping;
  stopping.sa_handler = stop;
  sigemptyset(&stopping.sa_mask);
  stopping.sa_flags = 0;
  sigaction(SIGINT, &stopping, nullptr);


  real_time_tools::Spinner spinner;
  spinner.set_frequency(2.0);
  running = true;
  double shared[7];
  
  while (running){

    shared_memory::get(REALTIME_TEST_SEGMENT_ID,
		       REALTIME_TEST_OBJECT_ID,
		       shared,7);

    int ticks = static_cast<int>(shared[0]);
    int switchs = static_cast<int>(shared[1]);
    double target_frequency = shared[2];
    double switch_frequency = shared[3];
    double average_frequency = shared[4];
    double current_frequency = shared[5];
    double worse_frequency = shared[6];

    std::cout << "nb ticks: " << ticks << "\n";
    std::cout << "nb switchs: " << switchs << "\n";
    std::cout << "frequencies:\n";
    std::cout << "\ttarget: " << target_frequency << "\n";
    std::cout << "\tprovokes switch: " << switch_frequency << "\n";
    std::cout << "\taverage: " << average_frequency << "\n";
    std::cout << "\tcurrent: " << current_frequency << "\n";
    std::cout << "\tworse: " << worse_frequency << "\n";
    std::cout << "\n\n";
    
    spinner.spin();

  }

}
