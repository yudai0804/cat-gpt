/**
 * @file led.h
 * @brief LEDを光らせるクラス
 */

#pragma once

#ifdef TARGET_ESP32
#include "Arduino.h"
#include "esp32-hal-gpio.h"
#endif
#include <stdint.h>

#include "status.h"

namespace peripheral {

class LED {
private:
  static constexpr uint8_t DT_MS = 1;
  uint16_t time_;
  uint16_t interval_;
  const uint8_t pin_;
  uint8_t status_;

public:
  LED(const uint8_t pin) : pin_(pin) {
    time_ = 0;
    status_ = 0;
    interval_ = 0;
  }

  void init() {
    pinMode(pin_, OUTPUT);
  }

  void on() { output(1); }

  void off() { output(0); }

  void output(const uint8_t is_on) {
    status_ = is_on;
    digitalWrite(pin_, status_);
  }

  /**
   * @brief 点滅
   *
   * @param frequency 周波数[Hz]は1000Hz以下で入力
   * @details 引数に0を入力した場合はLEDが消灯する
   * @return RET_STATUS
   */
  RET_STATUS blink(const float frequency) {
    if (frequency > 1000)
      return RET_STATUS_ARGUMENT_ERROR;

    // 0除算対策
    if (frequency == 0) {
      interval_ = 0;
      off();
    } else {
      // 1 / f * 1000 / 2
      interval_ = (uint16_t)(500.0f / (float)frequency);
    }

    return RET_STATUS_OK;
  }

  /**
   * @brief 割り込み内で呼ぶこと
   */
  void onInterrupt(void) {
    if (time_ <= 0xffff)
      time_ += DT_MS;
    if (time_ >= interval_ && interval_ != 0) {
      time_ = 0;
      status_ ^= 0x01;
      output(status_);
    }
  }
};

}  // namespace peripheral
