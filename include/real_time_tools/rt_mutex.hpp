/**
 * @file rt_mutex.hpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Expose real time OSs mutex through a common API.
 */
#ifdef __XENO__
#include <native/cond.h>
#include <native/mutex.h>
#include <native/timer.h>
#else
#include <pthread.h>
#include <sys/time.h>
#endif

#ifdef __XENO__
/** @brief Alias for the real time mutex. */
typedef RT_MUTEX rt_mutex;
/** @brief Alias for the real time condition variable. */
typedef RT_COND rt_cond;
#else
/** @brief Alias for the real time mutex. */
typedef pthread_mutex_t rt_mutex;
/** @brief Alias for the real time condition variable. */
typedef pthread_cond_t rt_cond;
#endif

/** @brief Initialize the mutex. */
static int rt_mutex_init(rt_mutex* mutex);
/** @brief Destroy the mutex. */
static int rt_mutex_destroy(rt_mutex* mutex);
/** @brief Lock the mutex. */
static int rt_mutex_lock(rt_mutex* mutex);
/** @brief Unlock the mutex. */
static int rt_mutex_unlock(rt_mutex* mutex);

static inline int rt_mutex_init(rt_mutex* mutex)
{
#ifdef __XENO__
    int res = rt_mutex_create(mutex, NULL);
    if (SL_RT_MUTEX_WARNINGS && res) sl_rt_warning("rt_mutex_create", res);
    return res;
#else
    return pthread_mutex_init(mutex, NULL);
#endif
}

static inline int rt_mutex_destroy(rt_mutex* mutex)
{
#ifdef __XENO__
    int res = rt_mutex_delete(mutex);
    if (SL_RT_MUTEX_WARNINGS && res) sl_rt_warning("rt_mutex_delete", res);
    return res;
#else
    return pthread_mutex_destroy(mutex);
#endif
}

static inline int rt_mutex_lock(rt_mutex* mutex)
{
#ifdef __XENO__
    int res = rt_mutex_acquire(mutex, TM_INFINITE);
    if (SL_RT_MUTEX_WARNINGS && res) sl_rt_warning("rt_mutex_acquire", res);
    return res;
#else
    return pthread_mutex_lock(mutex);
#endif
}

static inline int rt_mutex_unlock(rt_mutex* mutex)
{
#ifdef __XENO__
    int res = rt_mutex_release(mutex);
    if (SL_RT_MUTEX_WARNINGS && res) sl_rt_warning("rt_mutex_unlock", res);
    return res;
#else
    return pthread_mutex_unlock(mutex);
#endif
}
