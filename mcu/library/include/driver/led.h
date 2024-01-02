/**
 * @file led.h
 * @brief LEDを光らせるクラス
 */

#pragma once

#ifdef TARGET_ESP32
#include "Arduino.h"
#include "esp32-hal-gpio.h"
#endif

#include "common/common.h"
#include "timer/timer_base.h"
#include <stdint.h>

namespace driver {

class LED {
private:
  timer::TimerBase *timer_;
  timer::time_t interval_ = 0;
  uint8_t pin_ = 0;
  uint8_t status_ = 0;

public:
  LED(timer::TimerBase *timer) : timer_(timer) {}

  LED(timer::TimerBase *timer, const uint8_t pin) : timer_(timer), pin_(pin) {}

  void init() { DO_ESP32(pinMode(pin_, OUTPUT)); }

  void output(const uint8_t is_on) {
    status_ = is_on;
    DO_ESP32(digitalWrite(pin_, status_));
  }

  void on() { output(1); }

  void off() { output(0); }

  /**
   * @brief 点滅する。
   * @param interval 点滅の間隔[ms]
   * @details interval=500の場合、500ms ONになって、500ms 0FFになる
   * intarval=0の場合、消灯する
   */
  void blink(const timer::time_t interval) {
    interval_ = interval;
    // 出力をOFFにする。
    // interval=0の場合はここで設定された出力がonInterrupt内で更新されることはない
    off();
    // タイマーをリセット
    timer_->reset();
  }

  /**
   * @brief 割り込み内で呼ぶこと
   * またこの関数を呼ぶ際は割り込み内でタイマーの更新処理が終わった後に呼ぶこと
   */
  void onInterrupt(void) {
    bool is_longer_than_interval = (timer_->getElapsedTime() >= interval_);
    bool is_timer_enable = (interval_ != 0);
    if (is_longer_than_interval && is_timer_enable) {
      // xorをすると0と1が交互に切り替わって、Lチカの動作になる
      status_ ^= 0x01;
      output(status_);
      timer_->reset();
    }
  }
};

} // namespace driver