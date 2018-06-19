#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>


namespace rtpreempt_tools {

  // warning : initial version, copy pasted from : https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
  // I did not study things now, so this likely needs improvement
  // (alternative: https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example)

  // note: if failed as mlockall, run executable with sudo.

  int create_realtime_thread(pthread_t &thread,void*(*thread_function)(void*));


}
