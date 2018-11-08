/**
 * \file test_device.cpp
 * \brief Device unit tests
 * \author Maximilien Naveau
 * \date 2018
 *
 * This file implements a suit of unit tests for the Device class.
 * @see https://git-amd.tuebingen.mpg.de/amd-clmc/ci_example/wikis/catkin:-how-to-implement-unit-tests
 */

#include <gtest/gtest.h>

/**
 * @brief The DISABLED_TestRealTimeTools class is used to disable test.
 */
class DISABLED_TestRealTimeTools : public ::testing::Test {};

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

}
