
#include <eigen3/Eigen/Core>
#include <gtest/gtest.h>

#include <real_time_tools/threadsafe/threadsafe_timeseries.hpp>
#include <real_time_tools/timer.hpp>
#include "real_time_tools/thread.hpp"
#include "real_time_tools/mutex.hpp"

using namespace real_time_tools;

typedef Eigen::Matrix<double, 20, 20> Type;

/**
 * @brief This class contain the thread data
 */
class ThreadData
{
public:
    /**
     * @brief Construct a new ThreadData object
     * 
     * @param time_series_length is the size of the each buffer
     */
    ThreadData(size_t time_series_length): length_(10000), timeseries_(time_series_length)
    {
        n_outputs_ = 10;
        srand(0);
        inputs_.resize(length_, Type::Random());
        outputs_.resize(n_outputs_, std::vector<Type>(length_));
        output_indices_.resize(n_outputs_);
    }
    /** @brief is the number of input data*/
    size_t length_;
    /** @brief is the number of output data*/
    size_t n_outputs_;
    /** @brief Is the input data buffer*/
    std::vector<Type> inputs_;
    /** @brief Is the mutex that provide safe access to the ouputs*/
    real_time_tools::RealTimeMutex outputs_mutex_;
    /** @brief The output data buffer*/
    std::vector<std::vector<Type>> outputs_;
    /** @brief The thread safe time series data buffer*/
    real_time_tools::ThreadsafeTimeseries<Type> timeseries_;
    /** @brief is the indices of the buffer to be used in each threads*/
    std::vector<size_t> output_indices_;
};

/**
 * @brief This class is used to extract the data from a thread
 */
class OutputThreadData
{
public:
    /** @brief data_ This is a pointer to the data buffer */
    ThreadData* data_;
    /** @brief output_index_ This is the index in the data buffer */
    size_t output_index_;
};

/**
 * @brief This class gives the pointer to the data buffer to the thread and
 * define the speed of the thread.
 */
class InputThreadData
{
public:
    /** @brief Data buffer */
    ThreadData* data_;
    /** @brief slow down the writting thread */
    bool slow_;
};

void * timeseries_to_output(void* void_ptr)
{
    OutputThreadData& thread_data = *static_cast<OutputThreadData*>(void_ptr);
    size_t length = thread_data.data_->length_;
    size_t output_index = thread_data.output_index_;
    std::vector<std::vector<Type>>& outputs = thread_data.data_->outputs_;
    real_time_tools::RealTimeMutex& ouputs_mutex = thread_data.data_->outputs_mutex_;
    real_time_tools::ThreadsafeTimeseries<Type>& timeseries = thread_data.data_->timeseries_;

    Timer logger;
    logger.set_memory_size(length);
    logger.set_name("timeseries_to_output_" + std::to_string(output_index));

    for(size_t i = 0; i < length; i++)
    {
        Type element;
        real_time_tools::ThreadsafeTimeseries<Type>::Index timeindex = i;
        element = timeseries[timeindex];
        ouputs_mutex.lock();
        outputs[output_index][i] = element;
        ouputs_mutex.unlock();
        logger.tac_tic();
    }
    // logger.print_statistics();
    return nullptr;
}

void * input_to_timeseries(void* void_ptr)
{
    InputThreadData& thread_data = *static_cast<InputThreadData*>(void_ptr);
    size_t length = thread_data.data_->length_;
    std::vector<Type>& inputs = thread_data.data_->inputs_;
    real_time_tools::ThreadsafeTimeseries<Type>& timeseries = thread_data.data_->timeseries_;

    Timer logger;
    logger.set_memory_size(length);
    std::string suffix = "";
    if(thread_data.slow_){suffix = "_slow";}
    logger.set_name("input_to_timeseries" + suffix);

    
    for(size_t i = 0; i < length; i++)
    {
        timeseries.append(inputs[i]);
        logger.tac_tic();
        if(thread_data.slow_)
        {
            real_time_tools::Timer::sleep_ms(1.0);
        }
    }
    // logger.print_statistics();
    return nullptr;
}


bool test_threadsafe_timeseries_history(bool slow)
{   
    size_t time_series_size = 10000;
    if (slow)
    {
        time_series_size = 100;
    }
    ThreadData data(time_series_size);
    
    std::vector<OutputThreadData> output_data(data.n_outputs_);
    InputThreadData input_data;
    input_data.data_ = &data;
    input_data.slow_ = slow;

    std::vector<RealTimeThread> threads(data.n_outputs_ + 1);
    for(size_t i = 0; i < data.output_indices_.size(); i++)
    {
        // We provide the thread with all the infos needed
        output_data[i].data_ = &data;
        output_data[i].output_index_ = i;
        // We start the consumers thread
        threads[i].create_realtime_thread(
          &timeseries_to_output, &output_data[i]);
    }
    // We start the feeding thread
    threads.back().create_realtime_thread(&input_to_timeseries, &input_data);
    
    // wait for all the thread to finish
    for(size_t i = 0; i < threads.size(); i++)
    {
        threads[i].join();
    }

    // check that the outputs written by the individual threads
    // correspond to the input.
    for(size_t i = 0; i < data.n_outputs_; i++)
    {
        EXPECT_TRUE(data.inputs_ == data.outputs_[i]);
    }

    // sanity check
    data.inputs_[0](0,0) = 33.;
    EXPECT_FALSE(data.inputs_ == data.outputs_[0]);

    return true;
}


TEST(threadsafe_timeseries, full_history)
{   
    bool res = test_threadsafe_timeseries_history(false);
    EXPECT_TRUE(res);
}

TEST(threadsafe_timeseries, partial_history)
{
    bool res = test_threadsafe_timeseries_history(true);
    EXPECT_TRUE(res);
}

