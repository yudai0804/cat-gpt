/**
 * @file timer_1ms.h
 * @brief 制御周期1msのタイマー
 */

#pragma once

#include "timer/timer_base.h"
#include <functional>

namespace timer {

/**
 * @brief 1msの割り込み内で呼び出すこと
 */
void Timer1ms_update();
time_t Timer1ms_getCurrentTime();

class Timer1ms : public TimerBase {
public:
  Timer1ms();
  time_t getCurrentTime() { return Timer1ms_getCurrentTime(); }
};

} // namespace timer
