/**
 * @file switch.h
 * @brief GPIO入力を制御するクラス
 */

#pragma once
#ifdef TARGET_ESP32
#include "esp32-hal-gpio.h"
#include <Arduino.h>
#endif
#include "common/common.h"
#include <stdint.h>

namespace driver {

class Switch {
public:
  enum Status : uint8_t {
    DETECT_NO = 0,
    DETECT_UNDER_5MS,
    DETECT_UNDER_10MS,
    DETECT_UNDER_25MS,
    DETECT_UNDER_100MS,
    DETECT_UNDER_250MS,
    DETECT_UNDER_1000MS,
    DETECT_MORE_1000MS,
  };

private:
  static constexpr uint8_t DT_MS = 1;
  uint8_t pin_ = 0;
  uint8_t is_pullup_ = 0;
  Status status_ = DETECT_NO;
  uint16_t time_ = 0;

public:
  Switch(const uint8_t pin, const uint8_t is_pullup)
      : pin_(pin), is_pullup_(is_pullup) {}
  void init() { DO_ESP32(pinMode(pin_, INPUT)); }

  /**
   * @brief
   * @note is_pullup_が何かの間違いで1より大きい値だった場合死ぬので注意
   * @return
   */
  uint8_t read() {
    uint8_t ret = 0;
    DO_ESP32(ret = digitalRead(pin_));
    return ret ^ is_pullup_;
  }

  void onInterrupt() {
    if (read() == 0) {
      time_ = 0;
      status_ = DETECT_NO;
      return;
    }
    if (time_ < 0xffff)
      time_ += DT_MS;
    // センサーが反応した時間に応じてstatus_を更新
    if (time_ < 5) {
      status_ = DETECT_UNDER_5MS;
    } else if (time_ < 10) {
      status_ = DETECT_UNDER_10MS;
    } else if (time_ < 25) {
      status_ = DETECT_UNDER_25MS;
    } else if (time_ < 100) {
      status_ = DETECT_UNDER_100MS;
    } else if (time_ < 250) {
      status_ = DETECT_UNDER_250MS;
    } else if (time_ < 1000) {
      status_ = DETECT_UNDER_1000MS;
    } else {
      status_ = DETECT_MORE_1000MS;
    }
  }
  Switch::Status getStatus() { return status_; }
};

} // namespace driver
