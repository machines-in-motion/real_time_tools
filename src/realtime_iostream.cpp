#include <real_time_tools/realtime_iostream.hpp>


namespace real_time_tools {


bool create_directory(std::string path)
{
  return boost::filesystem::create_directory(path);
}

std::string get_home_dir()
{
  return std::string(getenv("HOME")) + "/";
}

} //namespace real_time_tools
