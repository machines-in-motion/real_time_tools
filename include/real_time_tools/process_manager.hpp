/**
 * @file process_manager.hpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-05-06
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#ifdef  XENOMAI
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
  #include <sched.h>
#endif

namespace real_time_tools {

bool fix_current_process_to_cpu(int cpu_affinity, int pid);

}

#endif // PROCESS_MANAGER