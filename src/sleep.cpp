#include "real_time_tools/sleep.hpp"

namespace real_time_tools
{

#ifdef XENOMAI

  int microsleep(int microseconds)
  {
    // rt_task_sleep is in nanoseconds
    return rt_task_sleep(microseconds*1e3);
  }

#else

  int microsleep(int microseconds)
  {
    usleep(microseconds);
    return 0;
  }

#endif

}
