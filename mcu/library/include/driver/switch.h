/**
 * @file switch.h
 * @brief GPIO入力を制御するクラス
 */

#pragma once
#include "timer/timer.h"
#ifdef TARGET_ESP32
#include <Arduino.h>

#include "esp32-hal-gpio.h"
#endif
#include <stdint.h>

#include "common/common.h"
#include "timer/timer_base.h"

namespace driver {

enum SwitchStatus : uint8_t {
  DETECT_NO = 0,
  DETECT_UNDER_5MS,
  DETECT_UNDER_10MS,
  DETECT_UNDER_25MS,
  DETECT_UNDER_100MS,
  DETECT_UNDER_250MS,
  DETECT_UNDER_1000MS,
  DETECT_MORE_1000MS,
};

class Switch {
public:
private:
  timer::TimerBase *timer_;
  uint8_t pin_ = 0;
  uint8_t is_pullup_ = 0;
  SwitchStatus status_ = DETECT_NO;

public:
  Switch(timer::UseTimer use_timer, const uint8_t pin, const uint8_t is_pullup)
      : pin_(pin), is_pullup_(is_pullup) {
    timer_ = timer::createTimer(use_timer);
  }

  void init() {
    // pinModeやらなくても、GPIO入力なら動くみたい
    DO_ESP32(pinMode(pin_, INPUT));
  }

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
      timer_->reset();
      status_ = DETECT_NO;
      return;
    }
    timer::time_t time = timer_->getElapsedTime();
    // センサーが反応した時間に応じてstatus_を更新
    if (time < 5) {
      status_ = DETECT_UNDER_5MS;
    } else if (time < 10) {
      status_ = DETECT_UNDER_10MS;
    } else if (time < 25) {
      status_ = DETECT_UNDER_25MS;
    } else if (time < 100) {
      status_ = DETECT_UNDER_100MS;
    } else if (time < 250) {
      status_ = DETECT_UNDER_250MS;
    } else if (time < 1000) {
      status_ = DETECT_UNDER_1000MS;
    } else {
      status_ = DETECT_MORE_1000MS;
    }
  }
  SwitchStatus getStatus() { return status_; }
  timer::time_t getTime() { return timer_->getElapsedTime(); }
};

}  // namespace driver
