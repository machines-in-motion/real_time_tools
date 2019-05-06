/**
 * @file process_manager.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-05-06
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <vector>
#include "real_time_tools/process_manager.hpp"

namespace real_time_tools {

bool fix_current_process_to_cpu(int cpu_affinity, int pid)
{
#ifdef  XENOMAI
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
  if (cpu_affinity > -1)
  {
      cpu_set_t mask;
      pid_t process_pid = static_cast<pid_t>(pid);
      int status;

      CPU_ZERO(&mask);
      CPU_SET(cpu_affinity, &mask);
      status = sched_setaffinity(process_pid, sizeof(mask), &mask);
      if (status != 0)
      {
        return false;
      }
      return true;
  }  
  return false;
#endif
}

} // namespace