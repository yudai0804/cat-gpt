/**
 * @file information.h
 * @brief
 */

#pragma once

#include <stdint.h>

#include "common/common.h"

namespace communication {

// TODO: search_modeとappel_modeをちゃんと考える
struct Information {
private:
  uint8_t search_mode_;
  uint8_t appeal_mode_;
  float manual_velocity_;
  float manual_omega_;

public:
  Information() {
    search_mode_ = 0;
    appeal_mode_ = 0;
    manual_velocity_ = 0.0f;
    manual_omega_ = 0.0f;
  }

  RET setSearchMode(int search_mode) {
    search_mode_ = search_mode;
    return RET_OK;
  }

  RET setAppealMode(int appeal_mode) {
    appeal_mode_ = appeal_mode;
    return RET_OK;
  }

  RET setManualVelocity(float manual_velocity) {
    manual_velocity_ = manual_velocity;
    return RET_OK;
  }

  RET setManualOmega(float manual_omega) {
    manual_omega_ = manual_omega;
    return RET_OK;
  }

  uint8_t getSearchMode() { return search_mode_; }
  uint8_t getAppealMode() { return appeal_mode_; }
  float getManualVelocity() { return manual_velocity_; }
  float getManualOmega() { return manual_omega_; }
};
}  // namespace communication
