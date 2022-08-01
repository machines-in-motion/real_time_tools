/**
 * @file hard_spinner.cpp
 * @author Julian Viereck (jviereck@tuebingen.mpg.de)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2020, New York University and Max Planck
 * Gesellschaft.
 * @date 2020-11-26
 *
 * @brief This file implements a hard spinner to time a loop. Compared to the 
 *   Spinner class, this spinner uses the desired frequency for how often to
 *   spin instead of the desired waiting time.
 * 
 * \code{.txt}
 *    Spinner:     |-----1 ms-----| |-work-| |-----1 ms-----| |-work-|
 *    HardSpinner: |-|-work-|-----| |-|-work-|-----| |-|-work-|-----|
 * \endcode
 */

#ifndef HARD_SPINNER_HPP
#define HARD_SPINNER_HPP

#include <unistd.h>
#include <chrono>
#include <real_time_tools/spinner.hpp>

namespace real_time_tools
{
/**
 * @brief Class to have threads / loops running at a desired frequency
 */
class HardSpinner: Spinner
{
public:
    // create a spinner for the desired frequency
    HardSpinner();

    /**
     * @brief spin waits for the time such that successive calls to spin
     * will result in spin being called at the desired frequency.
     */
    void spin();
};

}  // namespace real_time_tools

#endif  // SPINNER_HPP
