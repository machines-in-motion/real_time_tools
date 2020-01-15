/**
 * @file
 * @license BSD 3-clause
 * @copyright Copyright (c) 2020, New York University and Max Planck
 *            Gesellschaft
 *
 * @brief Implementation of the CheckpointTimer class.
 */

#pragma once

#include <array>
#include <iostream>
#include <string>

#include "timer.hpp"

namespace real_time_tools
{
/**
 * @brief Timer to measure code execution time with "checkpoints"
 *
 * This timer is meant to be used for measuring execution time of a loop.  It
 * measures time between calls of the `start` method, so by calling this at the
 * beginning of the loop, you get the execution time of the full iteration.
 * Further, you can define "checkpoints" within the loop to measure time of
 * separate steps in the loop.  Call the `checkpoint` method after the code that
 * is associated with it.  For each checkpoint, the time elapsed since the last
 * checkpoint is measured (`start` counts as a checkpoint in this regard).
 *
 * Example:
 *
 * ~~~{.cpp}
 * CheckpointTimer<3> timer;
 *
 * while (condition)
 * {
 *     timer.start();
 *
 *     initialize()
 *     timer.checkpoint("initialize");
 *
 *     do();
 *     some();
 *     stuff();
 *     timer.checkpoint("do some stuff");
 *
 *     write_log();
 *     timer.checkpoint("logging");
 * }
 * ~~~
 *
 * @tparam NUM_CHECKPOINTS Number of checkpoints.
 * @tparam ENABLED Set to false, to disable timer.  Method calls will have no
 * effect (and should hopefully be optimized away by the compiler).
 */
template <size_t NUM_CHECKPOINTS, bool ENABLED = true>
class CheckpointTimer
{
public:
    CheckpointTimer();

    //! @brief Start timer iteration.
    void start();

    /**
     * @brief Set checkpoint for time measurement.
     *
     * Measures time from the last call of start() or checkpoint() until this
     * call.  The given name is used when printing the results.
     *
     * @param checkpoint_name Name of the checkpoint (used for printing results)
     */
    void checkpoint(const std::string &checkpoint_name);

    //! @brief Print results of time measurements.
    void print_statistics() const;

private:
    std::array<real_time_tools::Timer, NUM_CHECKPOINTS + 1> timers_;
    std::array<std::string, NUM_CHECKPOINTS + 1> checkpoint_names_;
    size_t current_checkpoint = 1;
};

#include "checkpoint_timer.hxx"
}  // namespace real_time_tools
