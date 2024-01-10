/**
 * @file timer_20ms.cpp
 * @brief 制御周期1ms用タイマー
 */

#include "timer/timer_20ms.h"

#include "timer/timer_base.h"

namespace timer {

static constexpr time_t DT = 1;
static time_t current_time = 0;

void Timer20ms_update() { current_time += DT; }

time_t Timer20ms_getCurrentTime() { return current_time; }

// コンストラクタ
Timer20ms::Timer20ms() : TimerBase(DT) {}

}  // namespace timer
