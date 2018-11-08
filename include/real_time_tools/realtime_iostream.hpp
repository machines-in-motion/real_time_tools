#ifndef RT_IOSTREAM_HH
#define RT_IOSTREAM_HH

#ifdef __XENO__

#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <native/cond.h>

#include <rtdk.h>
#include <rtdm/rtcan.h>

#else

#include <stdio.h>
#include <stdlib.h>

#define rt_fprintf fprintf
#define rt_printf printf

#endif // __XENO__

#endif // RT_IOSTREAM_HH

