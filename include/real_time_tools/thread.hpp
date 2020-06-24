/**
 * @file thread.hpp
 * @author Maximilien Naveau (mnaveau@tue.mpg.de)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-11-21
 */

#ifndef REALTIME_THREAD_CREATION_HPP
#define REALTIME_THREAD_CREATION_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

#ifdef XENOMAI
// you MAY need to happend "static" upon declaration
#define THREAD_FUNCTION_RETURN_TYPE void
#define THREAD_FUNCTION_RETURN_VALUE
#include <native/cond.h>
#include <native/mutex.h>
#include <native/pipe.h>
#include <native/sem.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <sys/mman.h>

#elif defined NON_REAL_TIME
#include <iostream>
#include <thread>
// you need to happend "static" upon declaration
#define THREAD_FUNCTION_RETURN_TYPE void*
#define THREAD_FUNCTION_RETURN_VALUE nullptr

#define rt_printf printf

#elif defined RT_PREEMPT
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
// you need to happend "static" upon declaration
#define THREAD_FUNCTION_RETURN_TYPE void*
#define THREAD_FUNCTION_RETURN_VALUE nullptr

#define rt_printf printf

#endif

namespace real_time_tools
{
/**
 * @brief This class is a data structure allowing the user to share
 * configurations among threads. These parameter allows you to generate
 * real threads in xenomai and rt_preempt. The same code is compatible with
 * Mac and ubuntu but will run non-real time threads.
 *
 * warning : initial version, copy pasted from :
 * https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
 * I did not study things now, so this likely needs improvement (alternative:
 * https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example)
 * note: if failed as mlockall, run executable with sudo or be part of the
 * real_time group or xenomai group.
 */
class RealTimeThreadParameters
{
public:
    /**
     * @brief Construct a new RealTimeThreadParameters object
     */
    RealTimeThreadParameters()
    {
        keyword_ = "real_time_thread";
        priority_ = 80;
#ifdef XENOMAI
        // see:
        // https://xenomai.org/documentation/xenomai-2.6/html/api/group__task.html#ga520e6fad1decc5beff58b394ff443265
        stack_size_ = 2000000;
        dedicated_cpu_id_ = -1;
        priority_ = 75;  // not too high to avoid competition with SL
#elif defined NON_REAL_TIME
        stack_size_ = -1;
#elif defined RT_PREEMPT
        stack_size_ = 50 * PTHREAD_STACK_MIN;
#endif
        cpu_id_.clear();
        delay_ns_ = 0;
        block_memory_ = true;
        cpu_dma_latency_ = 0;
    }
    /**
     * @brief Destroy the RealTimeThreadParameters object
     */
    ~RealTimeThreadParameters()
    {
    }

public:
    /**
     * @brief Used in xenomai to define the thread id
     */
    std::string keyword_;
    /**
     * @brief Defines the thread priority from 0 to 100
     */
    int priority_;
    /**
     * @brief Define the stack size
     */
    int stack_size_;
    /**
     * @brief Define the cpu affinity. Which means on which cpu(s) the thread
     * is going to run
     */
    std::vector<int> cpu_id_;
    /**
     * @brief indicate on which cpu the thread will run (xenomai only)
     */
    int dedicated_cpu_id_;
    /**
     * @brief @todo Unknow Xenomai parameter
     */
    int delay_ns_;
    /**
     * @brief Defines if the thread should block the memory in a "page" or if
     * several pages can be use. Switching memory page is time consumming and
     * a non real time operation.
     */
    bool block_memory_;

    /**
     * @brief Maximum desired latency of the CPU in microseconds. Set to 0 to
     * get best real-time performance. Set to any negative value if you do not
     * want the thread to change the CPU latency.
     *
     */
    int cpu_dma_latency_;
};

/**
 * @brief This class allows you to spawn thread. Its parameter are defined
 * above.
 */
class RealTimeThread
{
public:
    /**
     * @brief Construct a new ThreadInfo object
     */
    RealTimeThread();

    /**
     * @brief We do not allow copies of this object
     */
    RealTimeThread(const real_time_tools::RealTimeThread& other) = delete;

    /**
     * @brief Destroy the RealTimeThread object.
     */
    ~RealTimeThread();

/**
 * @brief create_realtime_thread spawns a real time thread if the OS allows
 * it.
 * @param[in] thread_function: the executing function for the thread.
 * @param[in] args: arguments to be passed to the thread.
 * @return the error code.
 */
#ifdef XENOMAI
    int create_realtime_thread(void (*thread_function)(void*),
                               void* args = nullptr);
#else
    int create_realtime_thread(void* (*thread_function)(void*),
                               void* args = nullptr);
#endif

    /**
     * @brief join join the real time thread
     * @return the error code.
     */
    int join();

    /**
     * @brief block_memory block the current and futur memory pages.
     * see
     * https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/memory#memory-locking
     * for further explanation.
     */
    void block_memory();

    /**
     * @brief Paramter of the real time thread
     */
    RealTimeThreadParameters parameters_;

private:
#if defined(XENOMAI)
    RT_TASK thread_;
#elif defined(NON_REAL_TIME)
    std::unique_ptr<std::thread> thread_;
#elif defined(RT_PREEMPT)
    std::unique_ptr<pthread_t> thread_;
#endif
};
}  // namespace real_time_tools

#endif  // REALTIME_THREAD_CREATION_HPP
