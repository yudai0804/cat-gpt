/**
 * @file timer_1ms.cpp
 * @brief 制御周期1ms用タイマー
 */

#include "timer/timer_1ms.h"
#include "timer/timer_base.h"

namespace timer {

static constexpr time_t DT = 1;
static time_t current_time = 0;

void Timer1ms_update() { current_time += DT; }

time_t Timer1ms_getCurrentTime() { return current_time; }

// コンストラクタ
Timer1ms::Timer1ms() : TimerBase(DT) {}

} // namespace timer
