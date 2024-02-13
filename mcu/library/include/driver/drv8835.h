/**
 * @file drv8835.h
 * @brief drv8835というモータードライバでモーターを駆動するクラス
 */

#pragma once

#include "sys/_stdint.h"
#ifdef TARGET_ESP32
#include <Arduino.h>

#include "esp32-hal-gpio.h"
#include "esp32-hal-ledc.h"
#endif

#include <stdint.h>

#include <algorithm>
#include <cstdlib>

#include "common/common.h"

namespace driver {

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
  enum Direction : int8_t { POSITIVE_DIRECTION = 1,
                            NEGATIVE_DIRECTION = -1 };

private:
  float max_output_ = 1.0f;
  float output_a_ = 0.0f;
  float output_b_ = 0.0f;
  uint8_t output_mode_ = 0;
  HardwareConfig hardware_;
  int8_t direction_a_ = POSITIVE_DIRECTION;
  int8_t direction_b_ = POSITIVE_DIRECTION;

public:
  DRV8835(const HardwareConfig hardware, const float max_output = 1.0f)
      : hardware_(hardware), max_output_(max_output) {}

  RET setDirection(Direction direction_a, Direction direction_b) {
    if ((direction_a == POSITIVE_DIRECTION || direction_a == NEGATIVE_DIRECTION) &&
        (direction_b == POSITIVE_DIRECTION || direction_b == NEGATIVE_DIRECTION)) {
      direction_a_ = direction_a;
      direction_b_ = direction_b;
      return RET_OK;
    } else {
      return RET_ARGUMENT_ERROR;
    }
  }

  RET setMaxOutput(const float max_output) {
    if (max_output < 0.0f || max_output > 1.0f)
      return RET_ARGUMENT_ERROR;

    max_output_ = max_output;
    return RET_OK;
  }

  RET outputMode(uint8_t is_enable) {
    if (is_enable == 1 || is_enable == 0) {
      output_mode_ = is_enable;
      DO_ESP32(digitalWrite(hardware_.mode_pin, output_mode_));
      return RET_OK;
    } else {
      return RET_ARGUMENT_ERROR;
    }
  }

  RET enableMotorDriver() { return outputMode(0); }
  RET disableMotorDriver() { return outputMode(1); }

  void init() {
    // modeの初期化
    DO_ESP32(pinMode(hardware_.mode_pin, OUTPUT));
    // pwmの初期化
    DO_ESP32(ledcSetup(hardware_.ain1_channel, hardware_.pwm_frequency,
                       hardware_.pwm_resolution_bit));
    DO_ESP32(ledcSetup(hardware_.ain2_channel, hardware_.pwm_frequency,
                       hardware_.pwm_resolution_bit));
    DO_ESP32(ledcSetup(hardware_.bin1_channel, hardware_.pwm_frequency,
                       hardware_.pwm_resolution_bit));
    DO_ESP32(ledcSetup(hardware_.bin2_channel, hardware_.pwm_frequency,
                       hardware_.pwm_resolution_bit));
    DO_ESP32(ledcAttachPin(hardware_.ain1_pin, hardware_.ain1_channel));
    DO_ESP32(ledcAttachPin(hardware_.ain2_pin, hardware_.ain2_channel));
    DO_ESP32(ledcAttachPin(hardware_.bin1_pin, hardware_.bin1_channel));
    DO_ESP32(ledcAttachPin(hardware_.bin2_pin, hardware_.bin2_channel));
    // 出力をonにする
    output(0, 0);
    enableMotorDriver();
  }

  /**
   * @brief 値の範囲は-max_output ~ +max_output
   *
   * @param output_a
   */
  void outputA(const float output_a) {
    // 値を範囲内にする(std::clampと同じ)
    // c++17に対応していないためstd::clampが使えなかった
    output_a_ = std::max(std::min(output_a, max_output_), -max_output_);
    uint32_t pwm = (uint32_t)(std::abs(output_a_) * hardware_.pwm_resolution);
    // directionを適用
    if (output_a_ * direction_a_ > 0) {
      DO_ESP32(ledcWrite(hardware_.ain1_channel, pwm));
      DO_ESP32(ledcWrite(hardware_.ain2_channel, 0));
    } else {
      DO_ESP32(ledcWrite(hardware_.ain1_channel, 0));
      DO_ESP32(ledcWrite(hardware_.ain2_channel, pwm));
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
    output_b_ = std::max(std::min(output_b, max_output_), -max_output_);
    uint32_t pwm = (uint32_t)(std::abs(output_b_) * hardware_.pwm_resolution);
    if (output_b_ * direction_b_ > 0) {
      DO_ESP32(ledcWrite(hardware_.bin1_channel, pwm));
      DO_ESP32(ledcWrite(hardware_.bin2_channel, 0));
    } else {
      DO_ESP32(ledcWrite(hardware_.bin1_channel, 0));
      DO_ESP32(ledcWrite(hardware_.bin2_channel, pwm));
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
  uint8_t getDirectionA() { return direction_a_; }
  uint8_t getDirectionB() { return direction_b_; }
};
}  // namespace driver
