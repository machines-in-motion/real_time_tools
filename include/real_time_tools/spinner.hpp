#ifndef SPINNER_HPP
#define SPINNER_HPP

#include <chrono>
#include <unistd.h>

namespace real_time_tools {

  // class to have threads / loops running at a desired frequency
  class Spinner {

  public :

    // create a spinner for the desired frequency
    Spinner();

    /**
     * @brief set_period sets the period of the loop in !!seconds!!
     * @param period in seconds.
     */
    void set_period(double period)
    {
      period_sec_ = period;
    }

    void set_frequency(double frequency)
    {
      period_sec_ = 1.0/frequency;
    }

    /**
     * @brief To be called at the beginning of the loop if the spinner is not
     * created just before.
     */
    void initialize();

    /**
     * @brief spin waits for the time such that successive calls to spin
     * will result in spin being called at the desired frequency
     */
    void spin();

    /**
     * @brief Predict the time the current thread is going to sleep.
     */
    double predict_sleeping_time();

  private:

    /**
     * @brief period_sec_ is the period of the loop in seconds
     */
    double period_sec_;

    /**
     * @brief next_date_sec_ is the date when the loop needs to wake up.
     */
    double next_date_sec_;
  };

}

#endif // SPINNER_HPP
