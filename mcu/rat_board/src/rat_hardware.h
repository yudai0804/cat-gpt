/**
 * @file rat_hardware.h
 * @brief
 */

#pragma once

#include <stdint.h>

#include "control/trapezoid.h"
#include "control/two_wheel.h"
#include "driver/VL53L0X.h"
#include "driver/buzzer.h"
#include "driver/drv8835.h"
#include "driver/led.h"
#include "driver/switch.h"
#include "driver/wrapper/tof.h"
#include "timer/timer.h"
#include "timer/timer_1ms.h"
#include "timer/timer_base.h"

namespace rat {

class Hardware {
public:
  static constexpr float NORMAL_VELOCITY = 0.5f;
  static constexpr float NORMAL_OMEGA = 0.5f;
  // マイコンのペリフェラル関係
  VL53L0X vl53l0x_;
  driver::Buzzer buzzer_{23, 0};
  // 森先輩がピンアサインを間違えたので修正
  driver::DRV8835::HardwareConfig drv8835_hardware_config_ = {
      //       .mode_pin = 22,
      //       .ain1_pin = 21,
      //       .ain2_pin = 19,
      //       .bin1_pin = 18,
      //       .bin2_pin = 5,
      .mode_pin = 18,
      .ain1_pin = 19,
      .ain2_pin = 21,
      .bin1_pin = 22,
      .bin2_pin = 5,
      .ain1_channel = 1,
      .ain2_channel = 2,
      .bin1_channel = 3,
      .bin2_channel = 4,
      .pwm_frequency = 1000,
      .pwm_resolution_bit = 10,
      .pwm_resolution = 1024};
  driver::DRV8835 drv8835_{drv8835_hardware_config_};
  driver::LED led_red_{timer::USE_TIMER_1MS, 12};
  driver::LED led_white_{timer::USE_TIMER_1MS, 14};
  driver::Switch limit_switch_{timer::USE_TIMER_1MS, 15, 1};
  driver::Switch ui_switch_{timer::USE_TIMER_1MS, 27, 1};
  driver::ToF tof_{&vl53l0x_};
  // 制御関連
  control::Trapezoid trapezoid_velocity_{1.0f, 0.01f};
  control::Trapezoid trapezoid_omega_{1.0f, 0.01f};
  control::TwoWheel two_wheel_{1.0f};

public:
  Hardware() {}
  void init() {
    Serial.begin(115200);
    Wire.begin(16, 17, 100000);
    buzzer_.init();
    drv8835_.init();
    led_red_.init();
    led_white_.init();
    limit_switch_.init();
    ui_switch_.init();
    // 一旦コメントアウト
    // tof_.init();
    printf("rat hardware initialize finish\r\n");
  }

  void onInterrupt1ms() {
    led_red_.onInterrupt();
    led_white_.onInterrupt();
    limit_switch_.onInterrupt();
    ui_switch_.onInterrupt();
  }

  void onInterruptForToF() {
    // tofだけ実行に時間がかかるため、別の割り込みで動かす
    tof_.onInterrupt();
  }

  void resetTrapezoid() {
    trapezoid_velocity_.reset();
    trapezoid_omega_.reset();
  }

  void runByVelocity(const float velocity, const float omega) {
    float v = trapezoid_velocity_.calculate(velocity);
    float _omega = trapezoid_omega_.calculate(omega);
    two_wheel_.calculateByVelocity(v, _omega);
    drv8835_.output(two_wheel_.getVl(), two_wheel_.getVr());
  }

  void runByOmega(const float omega, const float turning_radius) {
    float _omega = trapezoid_omega_.calculate(omega);
    two_wheel_.calculateByOmega(_omega, turning_radius);
    drv8835_.output(_omega, turning_radius);
  }

  void stop() {
    resetTrapezoid();
    drv8835_.output(0.0f, 0.0f);
  }

  void beep(driver::Buzzer::MusicScale scale, uint32_t time) {
    buzzer_.beep(scale, time);
  }

  void straight(float velocity = NORMAL_VELOCITY) {
    runByVelocity(velocity, 0.0f);
  }

  void rotate(float omega = NORMAL_OMEGA) {
    runByVelocity(omega, 0);
  }

  uint16_t getDistance() { return tof_.getDistance(); }
  driver::SwitchStatus getLimitSwitch() { return limit_switch_.getStatus(); }
};
}  // namespace rat
