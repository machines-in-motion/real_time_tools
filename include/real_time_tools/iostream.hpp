#ifndef RT_IOSTREAM_HH
#define RT_IOSTREAM_HH

#include <boost/filesystem.hpp>

#if defined(XENOMAI)

#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <native/cond.h>

#include <rtdk.h>
#include <rtdm/rtcan.h>

#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)

#include <stdio.h>
#include <stdlib.h>

#define rt_fprintf fprintf
#define rt_printf printf

#endif


namespace real_time_tools {

std::string get_log_dir(std::string app_name);

bool create_directory(std::string path);

std::string get_home_dir();

} // namespace real_time_tools

#endif // RT_IOSTREAM_HH

