/**
 * @file realtime_iostream.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 * 
 * @brief implement some cross platform utilities to create folder or detect the
 * home folder, etc.
 */
#include <sstream>

#include "real_time_tools/timer.hpp"
#include "real_time_tools/iostream.hpp"

namespace real_time_tools {

std::string get_log_dir(std::string app_name)
{
  std::string log_dir;

  std::string home_dir = real_time_tools::get_home_dir();
  std::string date_dir = real_time_tools::Timer::get_current_date_str() + "/";

  log_dir = home_dir + app_name + "/" + date_dir;

  real_time_tools::create_directory(home_dir + app_name);
  real_time_tools::create_directory(log_dir);

  return log_dir;
}

bool create_directory(std::string path)
{
  return boost::filesystem::create_directory(path);
}

std::string get_home_dir()
{
  return std::string(getenv("HOME")) + "/";
}

} //namespace real_time_tools
