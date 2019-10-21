#ifndef MUTEX_HPP
#define MUTEX_HPP

#if defined(XENOMAI)
  #include <native/mutex.h>
  #include <native/cond.h>
  #include <native/timer.h>
#elif defined(RT_PREEMPT)
  #include <pthread.h>
  #include <sys/time.h>
#else // defined(NON_REAL_TIME)
  #include <mutex>
  #include <condition_variable>
#endif

namespace real_time_tools{

#if defined(XENOMAI)
  typedef RT_MUTEX RealTimeMutex_t;
  typedef RT_COND rt_cond;
#elif defined(RT_PREEMPT)
  typedef pthread_mutex_t RealTimeMutex_t;
  typedef pthread_cond_t rt_cond;
#else
  typedef std::mutex RealTimeMutex_t;
  typedef std::condition_variable rt_cond;
#endif

/**
 * @brief This class uses the real-time API of xenomai and posix to implement
 * mutexes.
 */
class RealTimeMutex{
public:
  /**
   * @brief Construct a new RealTimeMutex object
   */
  RealTimeMutex()
  {
    mutex_ = nullptr;

    int res = 0;
#if defined(XENOMAI)
    mutex_ = new RT_MUTEX();
    res = rt_mutex_create(mutex_, nullptr);
#elif defined(RT_PREEMPT)
    mutex_ = new pthread_mutex_t();
    res = pthread_mutex_init(mutex_, nullptr);
#else // defined(NON_REAL_TIME)
    mutex_ = new std::mutex();
#endif
    if (res > 0)
    {
      rt_printf("RealTimeMutex::RealTimeMutex(): "
                "error while creating mutex with code %d", res);
    }
  }
  /**
   * @brief Destroy the RealTimeMutex object
   */
  ~RealTimeMutex()
  {
    int res = 0;
#if defined(XENOMAI)
    res = rt_mutex_delete(mutex_);
#elif defined(RT_PREEMPT)
    res = pthread_mutex_destroy(mutex_);
#else // defined(NON_REAL_TIME)
    if(mutex_)
    {
      delete(mutex_);
    }
#endif
    if (res > 0)
    {
      rt_printf("RealTimeMutex::~RealTimeMutex(): "
                "error while destroying mutex with code %d", res);
    }
    mutex_ = nullptr;  
  }
  /**
   * @brief lock the mutex.
   */
  void lock()
  {
    int res = 0;
#if defined(XENOMAI)
    res = rt_mutex_acquire(mutex_, TM_INFINITE);
#elif defined(RT_PREEMPT)
    res = pthread_mutex_lock(mutex_);
#else // defined(NON_REAL_TIME)
    mutex_->lock();
#endif
    if (res)
    {
      rt_printf("RealTimeMutex::lock(): "
                "error while locking mutex with code %d", res);
    }
  }
  /**
   * @brief unlock the mutex
   */
  void unlock()
  {
    int res = 0;
#if defined(XENOMAI)
    res = rt_mutex_release(mutex_);
#elif defined(RT_PREEMPT)
    res = pthread_mutex_unlock(mutex_);
#else // defined(NON_REAL_TIME)
    mutex_->unlock();
#endif
    if (res)
    {
      rt_printf("RealTimeMutex::unlock(): "
                "error while locking mutex with code %d", res);
    }
  }

private:
  /**
   * @brief This is the object which type chenge according to the OS this code
   * is compiled
   */
  RealTimeMutex_t* mutex_;
};

} // namespace

#endif // Header protection