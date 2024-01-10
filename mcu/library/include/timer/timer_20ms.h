/**
 * @file timer_20ms.h
 * @brief 制御周期20msのタイマー
 */

#pragma once

#include "timer/timer_base.h"

namespace timer {

/**
 * @brief 20msの割り込み内で呼び出すこと
 */
void Timer20ms_update();
time_t Timer20ms_getCurrentTime();

class Timer20ms : public TimerBase {
public:
  Timer20ms();
  time_t getCurrentTime() { return Timer20ms_getCurrentTime(); }
};

}  // namespace timer
