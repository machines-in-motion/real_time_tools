
#ifdef XENOMAI

#include <native/task.h>

#else

#include <unistd.h>

#endif

namespace real_time_tools
{

  /**
   * @brief sleeps for the specified 
   * microseconds. 
   * @return 0 on success, error code on failure
   * (may happen on xenomai)
   */
  int microsleep(int microseconds);

}
