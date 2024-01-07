/**
 * @file create_timer.cpp
 * @brief
 */

#include "timer/timer.h"
#include "timer/timer_1ms.h"
#include "timer/timer_base.h"

namespace timer {
TimerBase* createTimer(UseTimer use_timer) {
  TimerBase* ret = nullptr;
  switch (use_timer) {
    case UseTimer::USE_TIMER_1MS:
      ret = new Timer1ms();
      break;
  }
  return ret;
}

}  // namespace timer
