#include "real_time_tools/spinner.hpp"
#include "real_time_tools/realtime_thread_creation.hpp"
#include "real_time_tools/realtime_check.hpp"


void* thread_function(void*) {

  double frequency = 300.0;
  double switch_frequency = 290;
  
  real_time_tools::Realtime_check realtime_check(frequency,
						 switch_frequency);
  real_time_tools::Spinner spinner;
  spinner.set_frequency(frequency);
  
  
  for(int i=0;i<500;i++){
    realtime_check.tick();
    spinner.spin();
  }

  std::cout << "\n";
  real_time_tools::print_realtime_check(realtime_check);
  std::cout << "\n";

  return NULL;
}



int main(int , char* []) {

  real_time_tools::RealTimeThread thread;
  real_time_tools::create_realtime_thread(thread, thread_function);
  real_time_tools::join_thread(thread);
  
}

