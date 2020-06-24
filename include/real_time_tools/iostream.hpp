/**
 * @file iostream.hpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Tools for console message display.
 */
#ifndef RT_IOSTREAM_HH
#define RT_IOSTREAM_HH

#include <boost/filesystem.hpp>

#if defined(XENOMAI)

#include <native/cond.h>
#include <native/mutex.h>
#include <native/task.h>
#include <native/timer.h>

#include <rtdk.h>
#include <rtdm/rtcan.h>

#elif defined(RT_PREEMPT) || defined(NON_REAL_TIME)

#include <stdio.h>
#include <stdlib.h>

#define rt_fprintf fprintf
#define rt_printf printf

#endif

namespace real_time_tools
{
/**
 * @brief Get the logging directory based on a specific application. It creates
 * a direction in $HOME/app_name/YEAR_MONTH_DAY_HOUR_SECOND/ and return the
 * absolute path of this. It allows the user to dump data in different folders
 * everytime the user launch the application.
 *
 * @param app_name is the application name
 * @return std::string the absolute path to the log directory
 */
std::string get_log_dir(std::string app_name);

/**
 * @brief Create a directory.
 *
 * @param path is the path to be created
 * @return true if everything went well
 * @return false if a problem occur
 */
bool create_directory(std::string path);

/**
 * @brief Get the home directory path.
 *
 * @return std::string the home directory absolute path ending with a "/"
 */
std::string get_home_dir();

}  // namespace real_time_tools

#endif  // RT_IOSTREAM_HH
