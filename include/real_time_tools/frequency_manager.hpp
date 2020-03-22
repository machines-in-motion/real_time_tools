/**
 * @file frequency_manager.hpp
 * @author Vincent Berenz (vberenz@tue.mpg.de)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
 * @date 2020-03-22
 * 
 * @brief Tools for enforcing a desired frequency in a loop
 */

# pragma once

#include "real_time_tools/timer.hpp"

namespace real_time_tools {
  /**
   * @brief Class to have threads / loops running at a desired frequency
   */
  class FrequencyManager {

  public :

    // create a manager for the desired frequency
    FrequencyManager(double frequency);

    /**
     * @brief waits for the time such that successive calls to wait
     * will result in wait being called at the desired frequency
     * @return true if the desired frequency could be enforced
     */
    bool wait();

  private:

    double period_ms_;
    double previous_time_ms_;
    
  };

}

