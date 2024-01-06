/**
 * @file tof.h
 * @brief
 */

#pragma once

#include "driver/VL53L0X.h"
#include "sys/_stdint.h"

namespace driver {
class ToF {
private:
  VL53L0X *vl53l0x_;
  uint16_t distance_;

public:
  ToF(VL53L0X *vl53l0x) : vl53l0x_(vl53l0x) {}
  void init() {
    vl53l0x_->init();
    vl53l0x_->setTimeout(500);
    vl53l0x_->startContinuous();
  }

  void onInterrupt() {
    distance_ = vl53l0x_->readRangeContinuousMillimeters();
  }

  uint16_t getDistance() { return distance_; }
};
}  // namespace driver
