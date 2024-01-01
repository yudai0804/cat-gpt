/**
 * @file common.h
 * @brief 共通な処理など
 */

#pragma once

#include <stdint.h>

enum RET : uint8_t {
  RET_OK = 0,
  RET_ERROR = 1,
  RET_ARGUMENT_ERROR = 2,
  RET_TIMEOUT_ERROR = 3,
  RET_BUSY_ERROR = 4
};

#ifdef TARGET_ESP32
#define DO_ESP32(func) func
#else
#define DO_ESP32(func)
#endif
