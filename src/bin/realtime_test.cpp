/**
 * @file realtime_test.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Program: test the real time capabilities of a machine
 */

#include "real_time_tools/realtime_test.hpp"
#include <signal.h>
#include <atomic>
#include <memory>
#include "real_time_tools/realtime_check.hpp"
#include "real_time_tools/spinner.hpp"
#include "real_time_tools/thread.hpp"
#include "shared_memory/shared_memory.hpp"

/** @brief valid modes, creating subclasses of Computation to add new ones
 * (see below)*/
static int MATRIX_COMPUTATION_NO_EIGEN_64 = 1;

/** @brief Is the thread running? */
std::atomic<bool> RUNNING;

/** @brief Configuration of the test thread */
class Configuration {
 public:
  /** @brief mode of the thread */
  int mode;
  /** @brief thread frequency */
  double frequency;
  /** @brief bound on the achieved frequency */
  double switch_frequency;
};

/** @brief Abstract interface for some different thread computation */
class Computation {
 public:
  /** @brief Interface to some computation to perform in the real time thread */
  virtual void compute() = 0;
};

/** @brief Some specific computation based on matrix multiplication */
class Matrix_computation_no_eigen : public Computation {
 public:
  /**
   * @brief Construct a new Matrix_computation_no_eigen object
   *
   * @param size of the matrices
   */
  Matrix_computation_no_eigen(int size) : size(size) {
    m1 = new double*[size];
    m2 = new double*[size];
    m3 = new double*[size];
    for (int i = 0; i < size; i++) {
      m1[i] = new double[size];
      m2[i] = new double[size];
      m3[i] = new double[size];
      for (int j = 0; j < size; j++) {
        m1[i][j] = 1.0;
        m2[i][j] = 2.0;
        m3[i][j] = 0.0;
      }
    }
  }
  /** @brief class destructor */
  ~Matrix_computation_no_eigen() {
    for (int i = 0; i < size; i++) {
      delete[] m1[i];
      delete[] m2[i];
      delete[] m3[i];
    }
    delete[] m1;
    delete[] m2;
    delete[] m3;
  }
  /**
   * @brief Compute one element of the matrix multiplication
   * 
   * @param i \f$ i^th \f$ row
   * @param j \f$ j^th \f$ column
   * @return double result of the multiplication
   */
  double compute(int i, int j) {
    double v = 0;
    for (int index = 0; index < size; index++) {
      v += m1[i][index] * m2[index][j];
    }
    return v;
  }
  /** @brief compute the matrix multiplication. */
  void compute() {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        m3[i][j] = compute(i, j);
      }
    }
  }

 private:
  /** @brief The size of the matrices. */
  int size;
  /** @brief The first matrix. */
  double** m1;
  /** @brief The second matrix. */
  double** m2;
  /** @brief The multiplication of the first 2 matrices. */
  double** m3;
};

/** @brief This is the real time thread that perform the check and the
 * computations */
void* thread_function(void* v) {
  Configuration* config = (Configuration*)(v);

  std::shared_ptr<Computation> computation;

  if (config->mode == MATRIX_COMPUTATION_NO_EIGEN_64) {
    computation.reset(new Matrix_computation_no_eigen(64));
  }

  real_time_tools::Spinner spinner;
  spinner.set_frequency(config->frequency);
  real_time_tools::RealTimeCheck checker(config->frequency,
                                         config->switch_frequency);

  RUNNING = true;

  int ticks, switchs;
  double target_frequency, switch_frequency;
  double average_frequency, current_frequency, worse_frequency;

  double shared[7];
  for (int i = 0; i < 7; i++) {
    shared[i] = 0.0;
  }

  while (RUNNING.load()) {
    // computing
    computation->compute();

    // computing current frequency
    checker.tick();

    // getting observed frequencies
    checker.get_statistics(ticks, switchs, target_frequency, switch_frequency,
                           average_frequency, current_frequency,
                           worse_frequency);

    // putting observed frequency in shared memory
    shared[0] = static_cast<double>(ticks);
    shared[1] = static_cast<double>(switchs);
    shared[2] = target_frequency;
    shared[3] = switch_frequency;
    shared[4] = average_frequency;
    shared[5] = current_frequency;
    shared[6] = worse_frequency;
    shared_memory::set(REALTIME_TEST_SEGMENT_ID, REALTIME_TEST_OBJECT_ID,
                       shared, 7);

    // trying to run at desired frequency
    spinner.spin();
  }
}

/** @brief Display the usage in case of a miss-use. */
void print_usage() {
  std::cout << "usage: realtime_test mode target_frequency switch_frequency"
            << "\n";
  std::cout << "\tvalid modes: " << MATRIX_COMPUTATION_NO_EIGEN_64
            << " (matrix multiplication no eigen)\n";
}

/** @brief parse the input argument and configure which conputation should be
 * done */
bool set_config(int nb_args, char** args, Configuration& config) {
  if (nb_args != 4) {
    print_usage();
    return false;
  }

  config.mode = atoi(args[1]);
  config.frequency = atof(args[2]);
  config.switch_frequency = atof(args[3]);

  if (config.mode != MATRIX_COMPUTATION_NO_EIGEN_64) {
    std::cout << "invalid mode: " << config.mode << "\n";
    print_usage();
    return false;
  }

  return true;
}

/** @brief Delete the shared memeory */
void clean_memory() {
  shared_memory::delete_segment(REALTIME_TEST_SEGMENT_ID);
  shared_memory::clear_shared_memory(REALTIME_TEST_OBJECT_ID);
}

/** @brief stop the current thread. This method is called throw a "ctrl+c" */
void stop(int) { RUNNING = false; }

/** @brief This program evaulate the quality of the frequency tracking by a real
 * time thread. */
int main(int nb_args, char** argv) {
  // in case not correctly cleaned during previous run
  clean_memory();

  // exit on ctrl+c
  struct sigaction stopping;
  stopping.sa_handler = stop;
  sigemptyset(&stopping.sa_mask);
  stopping.sa_flags = 0;
  sigaction(SIGINT, &stopping, nullptr);

  // creating configuration based on args
  Configuration config;
  bool ok = set_config(nb_args, argv, config);

  if (ok) {
    std::cout << "\n\nctrl-c for exiting\n";
    std::cout << "run realtime_test_display to see stats\n";

    real_time_tools::RealTimeThread thread;
    real_time_tools::block_memory();
    real_time_tools::create_realtime_thread(thread, thread_function, &config);
    real_time_tools::join_thread(thread);

    clean_memory();
  }
}
