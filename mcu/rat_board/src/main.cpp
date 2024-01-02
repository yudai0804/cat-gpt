#include <Arduino.h>

#include "driver/led.h"
#include "freertos/FreeRTOS.h"
#include "timer/timer_1ms.h"
#include "timer/timer_base.h"

TimerHandle_t timer_1ms;
driver::LED<timer::Timer1ms> led_red(12);
driver::LED<timer::Timer1ms> led_white(14);

void timer1msHandler(void *param) {
  timer::Timer1ms_update();
  led_red.onInterrupt();
  led_white.onInterrupt();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  led_red.init();
  led_white.init();

  timer_1ms = xTimerCreate("TIM_1MS", 1, pdTRUE, NULL, timer1msHandler);
  led_red.blinkByInterval(500);
  led_white.blinkByFrequency(1);
  xTimerStart(timer_1ms, 0);
}

void loop() {
  Serial.printf("time = %d, red = %d, white = %d\r\n", timer::Timer1ms_getCurrentTime(), led_red.getStatus(), led_white.getStatus());
}
