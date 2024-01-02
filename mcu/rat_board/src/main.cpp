#include <Arduino.h>

#include "driver/buzzer.h"
#include "driver/led.h"
#include "driver/switch.h"
#include "freertos/FreeRTOS.h"
#include "timer/timer_1ms.h"
#include "timer/timer_base.h"

TimerHandle_t timer_1ms;
driver::Buzzer buzzer(23, 0);
driver::LED<timer::Timer1ms> led_red(12);
driver::LED<timer::Timer1ms> led_white(14);
driver::Switch<timer::Timer1ms> limit_switch(15, 1);

void timer1msHandler(void *param) {
  timer::Timer1ms_update();
  led_red.onInterrupt();
  led_white.onInterrupt();
  limit_switch.onInterrupt();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  led_red.init();
  led_white.init();
  limit_switch.init();
  buzzer.init();

  timer_1ms = xTimerCreate("TIM_1MS", 1, pdTRUE, NULL, timer1msHandler);
  led_red.blinkByInterval(500);
  led_white.blinkByFrequency(1);
  xTimerStart(timer_1ms, 0);
  // buzzer.beep(driver::Buzzer::F4, 1000);
}

void loop() {
  // Serial.printf("time = %d, red = %d, white = %d\r\n", timer::Timer1ms_getCurrentTime(), led_red.getStatus(), led_white.getStatus());
  Serial.printf("time = %d, status = %d\r\n", limit_switch.getTime(), limit_switch.getStatus());
}
