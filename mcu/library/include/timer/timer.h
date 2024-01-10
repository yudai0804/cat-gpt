/**
 * @file timer.h
 * @brief
 */

#pragma once

#include <stdint.h>

namespace timer {
using time_t = uint32_t;

enum UseTimer {
  USE_TIMER_1MS,
  USE_TIMER_20MS
};
}  // namespace timer
