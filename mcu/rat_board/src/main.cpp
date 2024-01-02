#include <Arduino.h>

#include "driver/buzzer.h"
#include "driver/drv8835.h"
#include "driver/led.h"
#include "driver/switch.h"
#include "freertos/FreeRTOS.h"
#include "timer/timer_1ms.h"
#include "timer/timer_base.h"

TimerHandle_t timer_1ms;
driver::Buzzer buzzer(23, 0);

driver::DRV8835::HardwareConfig drv8835_hardware_config = {
    .mode_pin = 22,
    .ain1_pin = 21,
    .ain2_pin = 19,
    .bin1_pin = 18,
    .bin2_pin = 5,
    .ain1_channel = 1,
    .ain2_channel = 2,
    .bin1_channel = 3,
    .bin2_channel = 4,
    .pwm_frequency = 1000,
    .pwm_resolution_bit = 10,
    .pwm_resolution = 1024};
driver::DRV8835 drv8835(drv8835_hardware_config);
driver::LED<timer::Timer1ms> led_red(12);
driver::LED<timer::Timer1ms> led_white(14);
driver::Switch<timer::Timer1ms> limit_switch(15, 1);

void timer1msHandler(void *param) {
  timer::Timer1ms_update();
  led_red.onInterrupt();
  led_white.onInterrupt();
  limit_switch.onInterrupt();
}

void controlMotorByKeyboard() {
  static float output_a = 0.0f;
  static float output_b = 0.0f;
  if (!Serial.available()) return;
  int c = Serial.read();
  switch (c) {
    case 'a':
      output_a -= 0.05f;
      drv8835.outputA(output_a);
      break;
    case 'A':
      output_a += 0.05f;
      drv8835.outputA(output_a);
      break;
    case 's':
      output_b -= 0.05f;
      drv8835.outputB(output_b);
      break;
    case 'S':
      output_b += 0.05f;
      drv8835.outputB(output_b);
      break;
    case 'e':
    case 'E':
      output_a = output_b = 0.0f;
      drv8835.output(output_a, output_b);
      break;
  }
  // 範囲制限等の処理でキー入力が反映されていない可能性があるので更新
  output_a = drv8835.getOutputA();
  output_b = drv8835.getOutputB();
  printf("a = %+3f, b = %+3f\r\n", output_a, output_b);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  buzzer.init();
  drv8835.init();
  led_red.init();
  led_white.init();
  limit_switch.init();

  timer_1ms = xTimerCreate("TIM_1MS", 1, pdTRUE, NULL, timer1msHandler);
  led_red.blinkByInterval(500);
  led_white.blinkByFrequency(1);
  xTimerStart(timer_1ms, 0);
  // buzzer.beep(driver::Buzzer::F4, 1000);
}

void loop() {
  controlMotorByKeyboard();
  // Serial.printf("time = %d, red = %d, white = %d\r\n", timer::Timer1ms_getCurrentTime(), led_red.getStatus(), led_white.getStatus());
  // Serial.printf("time = %d, status = %d\r\n", limit_switch.getTime(), limit_switch.getStatus());
}
