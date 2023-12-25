/**
 * @file drv8835.h
 * @brief drv8835というモータードライバでモーターを駆動するクラス
 */

#pragma once

#include "esp32-hal-gpio.h"
#include "esp32-hal-ledc.h"
#include "status.h"
#include <Arduino.h>
#include <algorithm>
#include <cstdlib>
#include <stdint.h>

namespace peripheral {

class DRV8835 {
public:
  struct HardwareConfig {
    uint8_t mode_pin;
    uint8_t ain1_pin;
    uint8_t ain2_pin;
    uint8_t bin1_pin;
    uint8_t bin2_pin;
    uint8_t ain1_channel;
    uint8_t ain2_channel;
    uint8_t bin1_channel;
    uint8_t bin2_channel;
    uint32_t pwm_frequency;
    uint8_t pwm_resolution_bit;
    uint32_t pwm_resolution;
  };

private:
  float max_output_;
  float output_a_, output_b_;
  uint8_t output_mode_;
  const HardwareConfig hardware_;

public:
  DRV8835(const HardwareConfig hardware, const float max_output = 1.0f)
      : hardware_(hardware), max_output_(max_output) {
    output_a_ = output_b_ = 0;
    output_mode_ = 0;
  }

  RET_STATUS setMaxOutput(const float max_output) {
    if (max_output < 0.0f || max_output > 1.0f)
      return RET_STATUS_ARGUMENT_ERROR;

    max_output_ = max_output;
    return RET_STATUS_OK;
  }

  RET_STATUS outputMode(uint8_t is_enable) {
    if (is_enable == 1 || is_enable == 0) {
      output_mode_ = is_enable;
      digitalWrite(hardware_.mode_pin, output_mode_);
      return RET_STATUS_OK;
    } else {
      return RET_STATUS_ARGUMENT_ERROR;
    }
  }

  RET_STATUS enableMotorDriver() { return outputMode(1); }
  RET_STATUS disableMotorDriver() { return outputMode(0); }

  void init() {
    // modeの初期化
    pinMode(hardware_.mode_pin, OUTPUT);
    // pwmの初期化
    // clang-format off
    ledcSetup(hardware_.ain1_channel, hardware_.pwm_frequency, hardware_.pwm_resolution_bit);
    ledcSetup(hardware_.ain2_channel, hardware_.pwm_frequency, hardware_.pwm_resolution_bit);
    ledcSetup(hardware_.bin1_channel, hardware_.pwm_frequency, hardware_.pwm_resolution_bit);
    ledcSetup(hardware_.bin2_channel, hardware_.pwm_frequency, hardware_.pwm_resolution_bit);
    ledcAttachPin(hardware_.ain1_pin, hardware_.ain1_channel);
    ledcAttachPin(hardware_.ain2_pin, hardware_.ain2_channel);
    ledcAttachPin(hardware_.bin1_pin, hardware_.bin1_channel);
    ledcAttachPin(hardware_.bin2_pin, hardware_.bin2_channel);
    // clang-format on
    disableMotorDriver();
  }

  /**
   * @brief 値の範囲は-max_output ~ +max_output
   *
   * @param output_a
   */
  void outputA(const float output_a) {
    // 値を範囲内にする(std::clampと同じ)
    // c++17に対応していないためstd::clampが使えなかった
    output_a_ = std::min(std::max(output_a, max_output_), -max_output_);

    if (output_a_ > 0) {
      ledcWrite(hardware_.ain1_channel, output_a_ * max_output_);
      ledcWrite(hardware_.ain2_channel, 0);
    } else {
      ledcWrite(hardware_.ain1_channel, 0);
      ledcWrite(hardware_.ain2_channel, -output_a_ * max_output_);
    }
  }

  /**
   * @brief 値の範囲は-max_output ~ +max_output
   *
   * @param output_b
   */
  void outputB(const float output_b) {
    // 値を範囲内にする(std::clampと同じ)
    // c++17に対応していないためstd::clampが使えなかった
    output_b_ = std::min(std::max(output_b, max_output_), -max_output_);

    if (output_b_ > 0) {
      ledcWrite(hardware_.bin1_channel, output_b_ * max_output_);
      ledcWrite(hardware_.bin2_channel, 0);
    } else {
      ledcWrite(hardware_.bin1_channel, 0);
      ledcWrite(hardware_.bin2_channel, -output_b_ * max_output_);
    }
  }

  /**
   * @brief 値の範囲は-max_output ~ +max_output
   *
   * @param output_a
   * @param output_b
   */
  void output(const float output_a, const float output_b) {
    outputA(output_a);
    outputB(output_b);
  }

  float getOutputA() { return output_a_; }
  float getOutputB() { return output_b_; }
  float getMaxOutput() { return max_output_; }
};

} // namespace peripheral
