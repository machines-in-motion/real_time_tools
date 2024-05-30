/**
 * @file hard_spinner.cpp
 * @author Julian Viereck (jviereck@tuebingen.mpg.de)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2020, New York University and Max Planck
 * Gesellschaft.
 * @date 2020-11-26
 *
 * @brief This file implements a hard spinner to time a loop.
 */

#include <real_time_tools/hard_spinner.hpp>
#include <real_time_tools/timer.hpp>

namespace real_time_tools
{
HardSpinner::HardSpinner()
: Spinner() { }

void HardSpinner::spin()
{
    Timer::sleep_until_sec(next_date_sec_);
    next_date_sec_ = next_date_sec_ + period_sec_;
}

}  // namespace real_time_tools
