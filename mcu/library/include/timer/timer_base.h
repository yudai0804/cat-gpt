/**
 * @file timer_base.h
 * @brief timerクラスの基底クラス
 */

#pragma once

#include <stdint.h>

namespace timer {
/**
 * @class TimerBase
 * @note 経過時間を計測するだけなので、uint32_tのオーバーフロー対策は不要
 * オーバーフローしても正常に動作します。
 * 例)
 *
 * uint32_t a = UINT32_MAX + 1;
 * uint32_t b = UINT32_MAX;
 * uint32_t c = a - b;
 * この場合cは1になります(正常に動作する)
 *
 */
class TimerBase {
protected:
  uint32_t current_ = 0;
  uint32_t previous_ = 0;
  uint32_t dt_ = 0;

public:
  TimerBase() {}
  TimerBase(const uint32_t dt) : dt_(dt) {}
  virtual void update() = 0;

  /**
   * @brief タイマーをリセットする
   */
  void reset() { previous_ = current_; }

  /**
   * @brief dtを返す。dtが設定されていない場合は0を返す。
   *
   * @return
   */
  uint32_t getDT() { return dt_; }

  /**
   * @brief 現在時刻を返す
   *
   * @return
   */
  uint32_t getCurrentTime() { return current_; }
  uint32_t getPreviousTime() { return previous_; }

  /**
   * @brief 最後にresetしてからの経過時間を返す
   * @note
   * オーバーフロー対策不要と言いながら、実は最後にresetしてからUINT32_MAX経過すると、elapsedTimeが0になるけど、細かいことは気にしない！
   *
   * @return
   */
  uint32_t getElapsedTime() { return current_ - previous_; }
};
} // namespace timer
