/**
 * @file process_manager.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief Allow us to fix the current process to a specific set of cpus.
 */

#include <vector>
#include "real_time_tools/realtime_iostream.hpp"
#include "real_time_tools/process_manager.hpp"

namespace real_time_tools {

bool fix_current_process_to_cpu(std::vector<int>& cpu_affinities, int pid)
{
#ifdef  XENOMAI
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
  if (cpu_affinities.size()>0)
  {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    for(unsigned i=0 ; i< cpu_affinities.size() ; ++i)
    {
      if(cpu_affinities[i] > -1)
      {
        CPU_SET(cpu_affinities[i], &mask);
      }      
    }
    pid_t process_pid = static_cast<pid_t>(pid);
    int ret = sched_setaffinity(process_pid, sizeof(mask), &mask);
    if (ret)
    {
      rt_printf("sched_setaffinity failed. Ret= %d\n", ret);
      return false;
    }
    else
    {
      return true;
    }
  }
  rt_printf("fix_current_process_to_cpu: Nothing to be done.\n");
  return true;
#endif
}

} // namespace