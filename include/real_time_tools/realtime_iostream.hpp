#ifndef RT_IOSTREAM_HH
#define RT_IOSTREAM_HH

#include <boost/filesystem.hpp>

#ifdef __XENO__

#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <native/cond.h>

#include <rtdk.h>
#include <rtdm/rtcan.h>

#else

#include <stdio.h>
#include <stdlib.h>

#define rt_fprintf fprintf
#define rt_printf printf

#endif // __XENO__


namespace real_time_tools {

bool create_directory(std::string path);

std::string get_home_dir();

} // namespace real_time_tools

#endif // RT_IOSTREAM_HH

