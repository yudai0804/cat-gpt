/**
 * @file status.h
 * @brief
 */

#pragma once

#include <stdint.h>

enum RET_STATUS : uint8_t {
  RET_STATUS_OK = 0,
  RET_STATUS_ERROR = 1,
  RET_STATUS_ARGUMENT_ERROR = 2,
  RET_STATUS_TIMEOUT_ERROR = 3,
  RET_STATUS_BUSY_ERROR = 4
};
