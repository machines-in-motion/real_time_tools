/**
 * @file process_manager.hpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-06
 *
 * @brief Tools to fix the CPU to specific processor.
 */

#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#ifdef XENOMAI
#elif defined(NON_REAL_TIME) || defined(RT_PREEMPT)
#include <sched.h>
#endif

#include <vector>

namespace real_time_tools
{
/**
 * @brief Pin an executing process to a specific CPU in order to
 * avoid jumps between CPUs.
 *
 * @param cpu_affinities is the index of the CPU one wants to pin the process
 * on.
 * @param pid is the PID of the current process.
 * @return true if everything went well.
 * @return false otherwise
 */
bool fix_current_process_to_cpu(std::vector<int>& cpu_affinities, int pid);

/**
 * @brief Set the _cpu_dma_latency objectWe can set the maximum CPU latency
 * for processes in micro seconds.
 *
 * @param max_latency_us is the maximum latency in micro-seconds.
 * @return true if everything went well.
 * @return false if something went wrong.
 */
bool set_cpu_dma_latency(int max_latency_us);

}  // namespace real_time_tools

#endif  // PROCESS_MANAGER
