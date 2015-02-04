#ifndef TIMER_POOL_HPP
#define TIMER_POOL_HPP

#include "Pool.hpp"

class TimerThread;
typedef Pool<TimerThread> TimerPool;
extern TimerPool timerPool;

#endif
