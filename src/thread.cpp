/**
 * @file thread.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Implement method to create and join threads
 */
#include "real_time_tools/thread.hpp"
#include <stdexcept>
#include "real_time_tools/process_manager.hpp"

namespace real_time_tools
{
#if defined RT_PREEMPT

RealTimeThread::RealTimeThread()
{
    thread_.reset(nullptr);
}

RealTimeThread::~RealTimeThread()
{
    join();
    thread_.reset(nullptr);
}

void* exec(void (*f)(void*), void* args)
{
    f(args);
    return nullptr;
}

/**
 * @brief rt_preempt_error_message id common message for all things that could
 * go wrong.
 */
const std::string rt_preempt_error_message(
    "NOTE: This program must be executed with special permission to get "
    "the required real time permissions.\n"
    "Either use sudo or be part of the \'realtime\' group"
    "Aborting thread creation.");

int RealTimeThread::create_realtime_thread(void* (*thread_function)(void*),
                                           void* args)
{
    if (thread_ != nullptr)
    {
        printf("Thread already running");
    }

    if (parameters_.cpu_dma_latency_ >= 0)
    {
        set_cpu_dma_latency(parameters_.cpu_dma_latency_);
    }

    thread_.reset(new pthread_t());

    if (parameters_.block_memory_)
    {
        block_memory();
    }

    struct sched_param param;
    pthread_attr_t attr;
    int ret;

    ret = pthread_attr_init(&attr);
    if (ret)
    {
        printf(
            "%s %d\n",
            ("init pthread attributes failed. Ret=" + rt_preempt_error_message)
                .c_str(),
            ret);
        return ret;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, parameters_.stack_size_);
    if (ret)
    {
        printf("%s %d\n",
               ("pthread setstacksize failed. Ret=" + rt_preempt_error_message)
                   .c_str(),
               ret);
        return ret;
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    // ret = pthread_attr_setschedpolicy(&attr, SCHED_RR); // WARNING LAAS is
    // using this one!!!!
    if (ret)
    {
        printf(
            "%s %d\n",
            ("pthread setschedpolicy failed. Ret=" + rt_preempt_error_message)
                .c_str(),
            ret);
        return ret;
    }
    param.sched_priority = parameters_.priority_;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret)
    {
        printf("%s %d\n",
               ("pthread setschedparam failed. Ret=" + rt_preempt_error_message)
                   .c_str(),
               ret);
        return ret;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret)
    {
        printf(
            "%s %d\n",
            ("pthread setinheritsched failed. Ret=" + rt_preempt_error_message)
                .c_str(),
            ret);
        return ret;
    }

    /* Create a pthread with specified attributes */
    ret = pthread_create(thread_.get(), &attr, thread_function, args);
    if (ret)
    {
        printf(
            "%s %d\n",
            ("create pthread failed. Ret=" + rt_preempt_error_message).c_str(),
            ret);
        return ret;
    }

    if (parameters_.cpu_id_.size() > 0)
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        for (unsigned i = 0; i < parameters_.cpu_id_.size(); ++i)
        {
            CPU_SET(parameters_.cpu_id_[i], &cpuset);
        }
        ret = pthread_setaffinity_np(*thread_, sizeof(cpu_set_t), &cpuset);
        if (ret)
        {
            printf("%s %d\n",
                   ("Associate thread to a specific cpu failed. Ret=" +
                    rt_preempt_error_message)
                       .c_str(),
                   ret);
        }

        int get_aff_error = 0;
        get_aff_error =
            pthread_getaffinity_np(*thread_, sizeof(cpu_set_t), &cpuset);
        if (get_aff_error)
        {
            printf("%s %d\n",
                   ("Check the thread cpu affinity failed. Ret=" +
                    rt_preempt_error_message)
                       .c_str(),
                   ret);
        }
        printf("Set returned by pthread_getaffinity_np() contained: ");
        for (unsigned j = 0; j < CPU_SETSIZE; j++)
        {
            if (CPU_ISSET(j, &cpuset))
            {
                printf("CPU %d, ", j);
            }
        }
        printf("\n");
    }
    return ret;
}

int RealTimeThread::join()
{
    int ret = 0;
    if (thread_ != nullptr)
    {
        /* Join the thread and wait until it is done */
        ret = pthread_join(*thread_, nullptr);
        if (ret)
        {
            printf("join pthread failed.\n");
        }
        thread_.reset(nullptr);
    }
    return ret;
}

void RealTimeThread::block_memory()
{
    /* Lock memory */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
        printf("mlockall failed: %m\n");
        exit(-2);
    }
}
#endif  // Defined RT_PREEMPT

/**********************************************************
 * TODO: Check the implementation of this thread creation *
 **********************************************************/
#if defined XENOMAI

RealTimeThread::RealTimeThread()
{
}

RealTimeThread::~RealTimeThread()
{
    join();
}

int RealTimeThread::create_realtime_thread(void (*thread_function)(void*),
                                           void* args)
{
    // initializing rt_printf,
    // nothing would get printed otherwise
    rt_print_auto_init(1);

    int ret;
    if (parameters_.dedicated_cpu_id_ >= 0)
    {
        ret = rt_task_spawn(
            &thread_,
            parameters_.keyword_.c_str(),
            parameters_.stack_size_,
            parameters_.priority_,
            T_FPU | T_JOINABLE | T_CPU(parameters_.dedicated_cpu_id_),
            thread_function,
            args);
    }
    else
    {
        ret = rt_task_spawn(&thread_,
                            parameters_.keyword_.c_str(),
                            parameters_.stack_size_,
                            parameters_.priority_,
                            T_FPU | T_JOINABLE,
                            thread_function,
                            args);
    }

    return ret;
}

int RealTimeThread::join()
{
    int ret = rt_task_join(&thread_);
    return ret;
}

void RealTimeThread::block_memory()
{
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
        printf("mlockall failed: %m\n");
        exit(-2);
    }
}

#endif  // Defined XENOMAI

/**********************************************************
 * TODO: Check the implementation of this thread creation *
 **********************************************************/
#if defined NON_REAL_TIME

RealTimeThread::RealTimeThread()
{
    thread_.reset(nullptr);
}

RealTimeThread::~RealTimeThread()
{
    join();
    thread_.reset(nullptr);
}

int RealTimeThread::create_realtime_thread(void* (*thread_function)(void*),
                                           void* args)
{
    printf("Warning this thread is not going to be real time.\n");

    /* Create a standard thread for non-real time OS */
    thread_.reset(new std::thread(thread_function, args));
    return 0;
}

int RealTimeThread::join()
{
    if (thread_ != nullptr)
    {
        if (thread_->joinable())
        {
            thread_->join();
        }
    }
    return 0;
}

void RealTimeThread::block_memory()
{
    // do nothing
}
#endif  // Defined NON_REAL_TIME

}  // namespace real_time_tools
