/**
 * @file timer_1ms.h
 * @brief 制御周期1msのタイマー
 */

#pragma once

#include "timer/timer_base.h"

namespace timer {

class Timer1ms : public TimerBase {
public:
  Timer1ms() : TimerBase(1) {}
  /**
   * @brief 1msの割り込み内でこの関数を呼び出すこと
   */
  void update() { current_ += dt_; }
};

} // namespace timer
