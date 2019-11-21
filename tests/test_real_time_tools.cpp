/**
 * @file test_real_time_tools.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2019-05-22
 * 
 * @brief This file implements a suit of unit tests for the real time tools.
 * @see https://git-amd.tuebingen.mpg.de/amd-clmc/ci_example/wikis/catkin:-how-to-implement-unit-tests
 */

#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include "real_time_tools/iostream.hpp"
#include "real_time_tools/timer.hpp"
#include "real_time_tools/thread.hpp"
#include "real_time_tools/spinner.hpp"

// We use this in the unnittest for code simplicity
using namespace real_time_tools;

/**
 * @brief The DISABLED_TestRealTimeTools class is used to disable test.
 */
class DISABLED_TestRealTimeTools : public ::testing::Test{};

/**
 * @brief The TestRealTimeTools class: test suit template for setting up
 * the unit tests for the Device.
 */
class TestRealTimeTools : public ::testing::Test {

public:
   TestRealTimeTools(): ::testing::Test()
   {}

protected:
  /**
   * @brief SetUp, is executed before the unit tests
   */
  void SetUp() {

  }

  /**
   * @brief TearDown, is executed after teh unit tests
   */
  void TearDown() {

  }

};

/**
 * @brief test_timer_constructor tests the constructor of the class
 */
TEST_F(TestRealTimeTools, test_timer_constructor)
{
  Timer my_timer;
  ASSERT_EQ(my_timer.get_avg_elapsed_sec(), 0.0);
  ASSERT_EQ(my_timer.get_std_dev_elapsed_sec(), 0.0);
  ASSERT_EQ(my_timer.get_min_elapsed_sec(),
            std::numeric_limits<double>::infinity());
  ASSERT_EQ(my_timer.get_max_elapsed_sec(),
            -std::numeric_limits<double>::infinity());
}

TEST_F(TestRealTimeTools, test_timer_setters)
{
  Timer my_timer;
  my_timer.set_memory_size(1);
  // I do not know what to test here should I access the size of the buffer?
}

#ifndef MAC_OS
TEST_F(TestRealTimeTools, test_timer_timespec_add_sec)
{
  struct timespec t;
  t.tv_sec = 1;
  t.tv_nsec = 1005601;
  Timer::timespec_add_sec(t, 0.0);
  ASSERT_EQ(t.tv_sec, 1);
  ASSERT_EQ(t.tv_nsec, 1005601);
}
#endif // MAC_OS

TEST_F(TestRealTimeTools, test_timer_tic_and_tac_and_sleep)
{
  Timer my_timer;
  my_timer.tic();
  my_timer.sleep_sec(1.0);
  double time_slept = my_timer.tac();
  // This works we got basycally 500 micro sec of error in the sleeping time in
  // non real time.
  ASSERT_NEAR(time_slept, 1.0, 0.0005);
}

struct TmpData{
  Timer timer_;
  double duration_;
};

void* rt_thread_for_test(void* my_timer_pointer)
{
  TmpData* tmp_data = static_cast<TmpData*>(my_timer_pointer);
  tmp_data->timer_.tic();
  tmp_data->timer_.sleep_sec(1.0);
  tmp_data->duration_ = tmp_data->timer_.tac();

  return nullptr;
}

#ifdef NON_REAL_TIME 
TEST_F(DISABLED_TestRealTimeTools, test_timer_tic_and_tac_and_sleep_in_real_time)
#else // NON_REAL_TIME 
TEST_F(TestRealTimeTools, test_timer_tic_and_tac_and_sleep_in_real_time)
#endif // NON_REAL_TIME 
{
  TmpData tmp_data ;
  RealTimeThread thread;
  thread.create_realtime_thread(&rt_thread_for_test, &tmp_data);
  thread.join();
  // This works we got basycally 100 micro sec of error in the sleeping time in
  // real time. It is 5 times lower than the non real time test.
  ASSERT_NEAR(tmp_data.duration_, 1.0, 0.0001);
}

void* set_bool_to_true(void* data)
{
  bool* converted_data = static_cast<bool*>(data);
  *converted_data = true;
  return nullptr;
}

TEST_F(TestRealTimeTools, test_thread_execution)
{
  bool data = false ;
  RealTimeThread thread;
  thread.create_realtime_thread(set_bool_to_true, &data);
  thread.join();
  // This works we got basycally 100 micro sec of error in the sleeping time in
  // real time. It is 5 times lower than the non real time test.
  ASSERT_TRUE(data);
}


TEST_F(TestRealTimeTools, test_timer_dump)
{
  for(unsigned i=0 ; i<1000 ; ++i)
  {
    Timer my_timer;
    my_timer.set_memory_size(1);
    my_timer.tic();
    double desired_sleeping_time = 0.0001;
    my_timer.sleep_sec(desired_sleeping_time);
    double time_slept = my_timer.tac();
    my_timer.dump_measurements("/tmp/test_timer_dump.dat");
    std::ifstream is ("/tmp/test_timer_dump.dat");
    double data = -1.0;
    double duration = -1.0;
    int index = -1;
    int count = 0;
    while(is >> data)  // Attempt read into x, return false if it fails
    {
      if(count%2==0)
      {
        index = data;
      }else{
        duration = data;
      }
      ++count;
    }
    ASSERT_EQ(index, 0);
    ASSERT_EQ(count, 2);
    ASSERT_NEAR(duration, time_slept, 1e-8);
  }
}

TEST_F(TestRealTimeTools, test_time_statistics)
{
  Timer my_timer;
  my_timer.set_memory_size(2);
  my_timer.tic();
  my_timer.sleep_sec(0.0001);
  double time_slept = my_timer.tac();
  ASSERT_EQ(my_timer.get_avg_elapsed_sec(), time_slept);
  ASSERT_EQ(my_timer.get_std_dev_elapsed_sec(), 0.0);
  ASSERT_EQ(my_timer.get_min_elapsed_sec(), time_slept);
  ASSERT_EQ(my_timer.get_max_elapsed_sec(), time_slept);
  my_timer.tic();
  my_timer.sleep_sec(0.01);
  double time_slept2 = my_timer.tac();
  double mean = 0.5 * (time_slept + time_slept2);
  double std_dev = std::sqrt(0.5 * (std::pow(time_slept-mean,2) +
                                    std::pow(time_slept2-mean,2)));
  ASSERT_EQ(my_timer.get_avg_elapsed_sec(), mean);
  ASSERT_EQ(my_timer.get_std_dev_elapsed_sec(), std_dev);
  ASSERT_EQ(my_timer.get_min_elapsed_sec(), time_slept);
  ASSERT_EQ(my_timer.get_max_elapsed_sec(), time_slept2);
}

TEST_F(TestRealTimeTools, test_iostream_create_directory)
{
  std::string folder = "/tmp/.real_time_tools_test";
  real_time_tools::create_directory(folder);
  ASSERT_TRUE(boost::filesystem::exists(folder));
}

TEST_F(TestRealTimeTools, test_iostream_get_home_dir)
{
  std::string home_dir = real_time_tools::get_home_dir();
  std::cout << home_dir << std::endl;
  ASSERT_TRUE(home_dir != "");
}

TEST_F(TestRealTimeTools, test_iostream_get_current_date_str)
{
  // visual check performed, it seems correct on rt_preempt
  std::cout << real_time_tools::Timer::get_current_date_str() << std::endl;
}

TEST_F(TestRealTimeTools, test_spinner_normal_behavior)
{
  // some parameters
  double period_sec = 0.2;
  int iteration = 5;
  
  // we create the spinner
  real_time_tools::Spinner spinner;
  spinner.set_period(period_sec);

  // we mesaure the time of the loop. This should be roughly the same as:
  // iteration * period_sec in seconds.
  real_time_tools::Timer timer;
  timer.tic();
  for(int i = 0 ; i <= iteration ; ++i)
  {
    spinner.spin();
  }
  double time_in_the_loop = timer.tac();
  double therotical_time_in_the_loop = period_sec * static_cast<double>(iteration);

  ASSERT_NEAR(time_in_the_loop, therotical_time_in_the_loop, 0.1*period_sec);
}

TEST_F(TestRealTimeTools, test_spinner_loop_too_slow)
{
  // some parameters
  double period_sec = 0.2;
  int nb_it = 5;
  
  // we create the spinner
  real_time_tools::Spinner spinner;
  spinner.set_period(period_sec);

  // we mesaure the time of the loop. This should be roughly the same as:
  // nb_it * period_sec in seconds.
  real_time_tools::Timer timer;
  timer.tic();
  for(int i = 0 ; i < nb_it ; ++i)
  {
    if(i == 0)
    {
      real_time_tools::Timer::sleep_sec(nb_it*period_sec);
    }
    spinner.spin();
  }
  double time_in_the_loop = timer.tac();
  double therotical_time_in_the_loop = period_sec * static_cast<double>(2 * nb_it - 1);

  ASSERT_NEAR(time_in_the_loop, therotical_time_in_the_loop, 0.1*period_sec);
}