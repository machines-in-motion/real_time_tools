/**
 * @file process_manager.hpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-06
 * 
 * @brief Tools to fix the CPU to specific processor.
 */

#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#ifdef  XENOMAI
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
  #include <sched.h>
#endif

namespace real_time_tools {

bool fix_current_process_to_cpu(std::vector<int>& cpu_affinities, int pid);

bool set_cpu_dma_latency(int max_latency_us);

}

#endif // PROCESS_MANAGER