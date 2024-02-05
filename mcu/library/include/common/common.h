/**
 * @file common.h
 * @brief 共通な処理など
 */

#pragma once

#include <stdint.h>

enum RET : uint8_t {
  RET_OK = 0,
  RET_WARNING,
  RET_ERROR,
  RET_ARGUMENT_ERROR,
  RET_TIMEOUT_ERROR,
  RET_BUSY_ERROR
};

#ifdef TARGET_ESP32
#define DO_ESP32(func) func
#else
#define DO_ESP32(func)
#endif
