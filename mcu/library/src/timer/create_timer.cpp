/**
 * @file create_timer.cpp
 * @brief
 */

#include "timer/timer.h"
#include "timer/timer_1ms.h"
#include "timer/timer_20ms.h"
#include "timer/timer_base.h"

namespace timer {
TimerBase* createTimer(UseTimer use_timer) {
  TimerBase* ret = nullptr;
  switch (use_timer) {
    case UseTimer::USE_TIMER_1MS:
      ret = new Timer1ms();
      break;
    case UseTimer::USE_TIMER_20MS:
      ret = new Timer20ms();
      break;
  }
  return ret;
}

}  // namespace timer
