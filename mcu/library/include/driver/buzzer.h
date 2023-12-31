/**
 * @file buzzer.h
 * @brief ブザーを駆動するクラス
 */

#pragma once

#ifdef TARGET_ESP32
#include "esp32-hal-ledc.h"
#include "esp32-hal.h"
#include <Arduino.h>
#endif
#include "common/common.h"
#include <stdint.h>

namespace driver {

class Buzzer {
public:
  enum MusicScale : uint32_t {
    DESABLE = 0,
    /** @brief ド4 */
    C4 = 261,
    /** @brief ド#4 */
    C_SHARP4 = 277,
    /** @brief レ4 */
    D4 = 293,
    /** @brief レ#4 */
    D_SHARP4 = 311,
    /** @brief ミ4 */
    E4 = 329,
    /** @brief ファ4 */
    F4 = 349,
    /** @brief ファ#4 */
    F_SHARP4 = 369,
    /** @brief ソ4 */
    G4 = 391,
    /** @brief ソ#4 */
    G_SHARP4 = 415,
    /** @brief ラ4 */
    A4 = 440,
    /** @brief ラ#4 */
    A_SHARP4 = 466,
    /** @brief シ4 */
    B4 = 493,
  };

private:
  uint8_t pin_ = 0;
  uint8_t channnel_ = 0;
  uint32_t frequency_ = 0;
  uint32_t time_ = 0;

public:
  Buzzer(const uint8_t pin, const uint8_t channel)
      : pin_(pin), channnel_(channel) {}

  /**
   * @brief
   * @note
   * 周波数や分解能の設定はledcWriteTone呼び出し時に行われるため、この関数内では行わなくてよい
   */
  void init() {
    DO_ESP32(ledcSetup(0, 12800, 10));
    DO_ESP32(ledcAttachPin(pin_, channnel_));
  }

  /**
   * @brief beep音を鳴らす
   *
   * @param frequency
   * @param time
   * @note 割り込み内で呼ばないこと
   */
  void beep(const uint32_t frequency, uint32_t time) {
    frequency_ = frequency;
    time_ = time;
    // TODO vTaskDelayのほうがいいかも
    DO_ESP32(ledcWriteTone(channnel_, frequency_));
    DO_ESP32(delay(time_));
  }
};
} // namespace driver
