/**
 * @file mutex.hpp
 * @author Maximilien Naveau
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-11-19
 * @brief This file implements a real time safe mutex with the dedicated
 * libraries. The API tries to fit the std API as much as possible.
 */

#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <string>

#if defined(XENOMAI)
#include <native/cond.h>
#include <native/mutex.h>
#include <native/timer.h>
#elif defined(RT_PREEMPT)
#include <pthread.h>
#include <sys/time.h>
#define rt_printf printf
#else  // defined(NON_REAL_TIME)
#include <condition_variable>
#include <mutex>
#define rt_printf printf
#endif

namespace real_time_tools
{
#if defined(XENOMAI)
/** @brief Alias for the real time mutex */
typedef RT_MUTEX RealTimeMutex_t;
/** @brief Alias for the real time condition variable */
typedef RT_COND rt_cond;
#elif defined(RT_PREEMPT)
/** @brief Alias for the real time mutex */
typedef pthread_mutex_t* RealTimeMutex_t;
/** @brief Alias for the real time condition variable */
typedef pthread_cond_t rt_cond;
#else
/** @brief Alias for the real time mutex */
typedef std::mutex* RealTimeMutex_t;
/** @brief Alias for the real time condition variable */
typedef std::condition_variable rt_cond;
#endif

/**
 * @brief This class uses the real-time API of xenomai and posix to implement
 * mutexes.
 */
class RealTimeMutex
{
public:
    /**
     * @brief Construct a new RealTimeMutex object
     */
    RealTimeMutex(std::string mutex_id = "")
    {
        mutex_id_ = mutex_id;

        int res = 0;
#if defined(XENOMAI)
        res = rt_mutex_create(&mutex_, mutex_id.c_str());
#elif defined(RT_PREEMPT)
        mutex_ = nullptr;
        mutex_ = new pthread_mutex_t();
        res = pthread_mutex_init(mutex_, nullptr);
#else  // defined(NON_REAL_TIME)
        mutex_ = nullptr;
        mutex_ = new std::mutex();
#endif
        if (res > 0)
        {
            rt_printf(
                "RealTimeMutex::RealTimeMutex(): "
                "error while creating mutex with code %d",
                res);
        }
    }
    /**
     * @brief Destroy the RealTimeMutex object
     */
    ~RealTimeMutex()
    {
        int res = 0;
#if defined(XENOMAI)
        res = rt_mutex_delete(&mutex_);
#elif defined(RT_PREEMPT)
        res = pthread_mutex_destroy(mutex_);
#else  // defined(NON_REAL_TIME)
        if (mutex_)
        {
            delete (mutex_);
        }
#endif
        if (res > 0)
        {
            rt_printf(
                "RealTimeMutex::~RealTimeMutex(): "
                "error while destroying mutex with code %d",
                res);
        }
#ifndef XENOMAI
        mutex_ = nullptr;
#endif
    }
    /**
     * @brief lock the mutex.
     */
    void lock()
    {
        int res = 0;
#if defined(XENOMAI)
        res = rt_mutex_acquire(&mutex_, TM_INFINITE);
#elif defined(RT_PREEMPT)
        res = pthread_mutex_lock(mutex_);
#else  // defined(NON_REAL_TIME)
        mutex_->lock();
#endif
        if (res)
        {
            rt_printf(
                "RealTimeMutex::lock(): "
                "error while locking mutex with code %d",
                res);
        }
    }
    /**
     * @brief unlock the mutex
     */
    void unlock()
    {
        int res = 0;
#if defined(XENOMAI)
        res = rt_mutex_release(&mutex_);
#elif defined(RT_PREEMPT)
        res = pthread_mutex_unlock(mutex_);
#else  // defined(NON_REAL_TIME)
        mutex_->unlock();
#endif
        if (res)
        {
            rt_printf(
                "RealTimeMutex::unlock(): "
                "error while locking mutex with code %d",
                res);
        }
    }

private:
    /**
     * @brief This is the object which type chenge according to the OS this code
     * is compiled
     */
    RealTimeMutex_t mutex_;

    /**
     * @brief Save the mutex id internally.
     */
    std::string mutex_id_;
};

}  // namespace real_time_tools

#endif  // Header protection
