/**
 * @file threadsafe_timeseries.hpp
 * @author Manuel Wuthrich (manuel.wuthrich@gmail.com)
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @brief
 * @version 0.1
 * @date 2018-11-27
 *
 * @copyright Copyright (c) 2018
 *
 */

#pragma once

#include <memory>
#include <vector>

#include "real_time_tools/timer.hpp"

#include <condition_variable>
#include <mutex>

namespace real_time_tools
{
/**
 * \brief implements a timeseries  \f$ X_{{oldest}:{newest}} \f$ which can
 * safely be accessed from multiple threads.
 *
 * this object has the following properties:
 * - an oldest timeindex \f$ oldest\f$,
 * - a newest timeindex \f$ newest \f$,
 * - a value \f$ X_i \f$ for each  \f$ i \in \{oldest, oldest + 1 , ...,
 * newest\} \f$,
 * - a length \f$length\f$
 * - and a maximum length \f$maxlength\f$
 */
template <typename Type = int>
class ThreadsafeTimeseries
{
public:
    /**
     * @brief Alias for the index type.
     */
    typedef long int Index;

    /**
     * @brief alias for the Timestamp.
     */
    typedef long double Timestamp;

    /*! \brief initializes to an empty timeseries with the given
     * \f$ maxlength \f$. when the first element will be inserted, it will
     * have index start_timeindex.
     *
     * this means that this timeseries contains no elements yet,
     * hence \f$ newest \f$ and \f$ oldest \f$ are not defined after
     * construction, there exist no elements and \f$length\f$ is zero
     */
    ThreadsafeTimeseries(size_t max_length, Index start_timeindex = 0);

    // accessors ---------------------------------------------------------------
    /*! \brief returns \f$ newest \f$. waits if the timeseries is empty.
     */
    virtual Index newest_timeindex() const;

    /*! \brief returns the number of element contained in the queue.
     */
    virtual Index count_appended_elements() const;

    /*! \brief returns \f$ oldest \f$. waits if the timeseries is empty.
     */
    virtual Index oldest_timeindex() const;

    /*! \brief returns \f$ X_{newest} \f$. waits if the timeseries is empty.
     */
    virtual Type newest_element() const;

    /*! \brief returns \f$ X_{timeindex} \f$. waits if the timeseries is empty
     * or if \f$timeindex > newest \f$.
     */
    virtual Type operator[](const Index &timeindex) const;

    /*! \brief returns the time in miliseconds when \f$ X_{timeindex} \f$
     * was appended. Waits if the timeseries is empty
     * or if \f$timeindex > newest \f$.
     */
    virtual Timestamp timestamp_ms(const Index &timeindex) const;

    /*! \brief returns the time in seconds when \f$ X_{timeindex} \f$
     * was appended. Waits if the timeseries is empty
     * or if \f$timeindex > newest \f$.
     */
    virtual Timestamp timestamp_s(const Index &timeindex) const;

    /*! \brief Wait until the defined time index is reached. If the input time
     *  is below the oldest time index that have been registered read an
     * exception is return.
     */
    virtual bool wait_for_timeindex(
        const Index &timeindex,
        const double &max_duration_s =
            std::numeric_limits<double>::quiet_NaN()) const;

    /*! \brief returns the length of the timeseries, i.e. \f$0\f$ if it is
     * empty, otherwise \f$newest - oldest +1 \f$.
     */
    virtual size_t length() const;

    /** @brief returns the maximum length of the time serie.
     *  @return size_t
     */
    virtual size_t max_length() const;

    /*! \brief returns boolean indicating whether new elements have been
     * appended since the last time the tag() function was called.
     */
    virtual bool has_changed_since_tag() const;

    // mutators ----------------------------------------------------------------
    /*! \brief tags the current timeseries, can later be used to check
     * whether new elements have been added
     */
    virtual void tag(const Index &timeindex);

    /*! \brief appends a new element to the timeseries, e.g. we go from
     * \f$ X_{1:10} \f$ to \f$ X_{1:11} \f$ (where \f$ X_{11}=\f$ element).
     * if the timeseries length is
     * already equal to its max_length, then the oldest element is discarded,
     * e.g. for a max_length = 10 we would go from \f$ X_{1:10} \f$
     * to \f$ X_{2:11} \f$.
     */
    virtual void append(const Type &element);

private:
    /*! @brief History of the values. */
    std::shared_ptr<std::vector<Type>> history_elements_;
    /*! @brief History of the headers. */
    std::shared_ptr<std::vector<Timestamp>> history_timestamps_;

    Index start_timeindex_;
    /*! @brief Oldest time index. */
    Index oldest_timeindex_;
    /*! @brief Newest time index. */
    Index newest_timeindex_;
    /*! @brief Tagged time index. */
    Index tagged_timeindex_;

    /**
     * @brief A condition variable that protect the data during copy and
     * reading.
     */
    mutable std::shared_ptr<std::condition_variable> condition_;
    /**
     * @brief A mutex variable that protect the data during copy and
     * reading.
     */
    mutable std::shared_ptr<std::mutex> mutex_;
};
}  // namespace real_time_tools

#include "real_time_tools/threadsafe/threadsafe_timeseries.hxx"
