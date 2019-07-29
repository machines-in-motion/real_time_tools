#include <iostream>
#include "real_time_tools/threadsafe/threadsafe_timeseries.hpp"
#include "real_time_tools/thread.hpp"


// instances of items will be added to the time series

class Item {

public:
  Item():d(0.0),i(0){}
  Item(double d, int i): d(d), i(i) {}
  double d;
  int i;

};


// configuration of the threads

struct Thread_config {
  bool stop;
  long int index_to_wait_for;
  real_time_tools::ThreadsafeTimeseries<Item> *tts;
  std::mutex *print_mutex;
};


std::mutex PRINT_MUTEX;

// add nb_items to the time series at given frequency
  
void add_items( int nb_items,
		double frequency,
		real_time_tools::ThreadsafeTimeseries<Item> &tts ) {

  double wait_time_seconds = 1.0 / frequency;
  
  for (int index=0;index<nb_items;index++){
    
    Item item(static_cast<double>(index),index);
    tts.append(item);

    PRINT_MUTEX.lock();
    std::cout << "write:\t" << item.d << "\t" << item.i << "\n";
    PRINT_MUTEX.unlock();
    
    usleep(wait_time_seconds*1e6);
    
  }

}


// will run in a thread. Wait for a specific index to have been added
// to the time series, print the corresponding item, and exit

void* futur_print(void* config){

  Thread_config* thread_config = static_cast<Thread_config*>(config);
  long int index_to_wait_for = thread_config->index_to_wait_for;
  real_time_tools::ThreadsafeTimeseries<Item> *tts = thread_config->tts;
  
  Item item = (*tts)[index_to_wait_for];
  long double time_stamp = tts->timestamp_ms(index_to_wait_for);

  thread_config->print_mutex->lock();
  std::cout << "\n\n**** received " << index_to_wait_for << " at time " << time_stamp << "****\n\n";
  thread_config->print_mutex->unlock();
  
}


// print items as they are added to the time series

void* live_print( void *config ) {

  bool first_element = true;
  
  Thread_config* thread_config = static_cast<Thread_config*>(config);

  long int index;
  
  while (! thread_config->stop){
    if (first_element){
      index = thread_config->tts->newest_timeindex();
    } else {
      index += 1;
    }
    // note : will wait for index if not already available
    Item item = (*(thread_config->tts))[index];

    thread_config->print_mutex->lock();
    std::cout << "\tread:\t" << item.d << "\t" << item.i << "\n";
    thread_config->print_mutex->unlock();
    
    first_element = false;
  }

}


// print current content of the time series

void print_full_content(real_time_tools::ThreadsafeTimeseries<Item> &tts){

  std::cout << "\n\n\t\ttime series content: \n";

  // how to iterate over all tts content ?
  /* 
  for (long int index : tts){
    Item item = tts[index];
    long double timestamp = tts.timestamp_ms(index);
    std::cout << index << "\t" << timestamp << "\t|\t" << item.d << "\t" << item.i << "\n";
    }*/
  
  std::cout << "\n";
  
}



int main(){

  // number of items in the time series. Older items are deleted from the series.
  int series_length = 10;
  
  real_time_tools::ThreadsafeTimeseries<Item> tts(series_length);

  // this thread will be used to print items "live", as they are added to tts
  real_time_tools::RealTimeThread live_print_thread;
  Thread_config live_print_config;
  live_print_config.stop = false; // thread will stop when 'stop' set to true
  live_print_config.tts = &tts;
  live_print_config.print_mutex = &PRINT_MUTEX;
  live_print_thread.create_realtime_thread(&live_print,&live_print_config);
  
  // this thread will be used to print a single item, corresponding
  // to the time index declared below
  real_time_tools::RealTimeThread futur_print_thread;
  Thread_config futur_print_config;
  futur_print_config.index_to_wait_for=15;
  futur_print_config.tts = &tts;
  futur_print_config.print_mutex = &PRINT_MUTEX;
  futur_print_thread.create_realtime_thread(&futur_print,&futur_print_config);
    
  for (int iteration=0; iteration<3 ; iteration++){

    PRINT_MUTEX.lock();
    std::cout << "\n\n---- iteration " << iteration << " ----\n\n";
    PRINT_MUTEX.unlock();
    
    // generating 20 items, 5 item per second.
    add_items(20,5.0,tts);

    // shows the full content of the time series.
    // because it is of size 10, only the last 10 items should be displayed
    print_full_content(tts);

  }
  
  // futur_print_thread is supposed to have exited after printing item number 15

  // stopping live_print_thread
  live_print_config.stop = true;
  live_print_thread.join();

}
