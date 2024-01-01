#include <Arduino.h>

#include "driver/VL53L0X.h"
#include "driver/buzzer.h"
#include "driver/led.h"
#include "driver/switch.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/timers.h"

driver::LED led_red(12);
driver::LED led_white(14);
driver::Switch sw(15, 1);
driver::Buzzer buzzer(23, 0);

const uint8_t sda = 16;
const uint8_t scl = 17;

uint32_t count = 0;

TimerHandle_t timer_1khz;

VL53L0X vl53l0x;

void timerHandler(void *param) {
  count++;
  led_red.onInterrupt();
  led_white.onInterrupt();
  sw.onInterrupt();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  led_red.init();
  led_white.init();
  sw.init();
  buzzer.init();

  timer_1khz = xTimerCreate("TIM_1KHZ", 1, pdTRUE, NULL, timerHandler);
  led_red.blink(1);
  led_white.blink(10);
  Wire.begin(sda, scl, 100000);
  vl53l0x.setTimeout(500);
  if (!vl53l0x.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {
    }
  }
  vl53l0x.setSignalRateLimit(0.01);
  vl53l0x.startContinuous();
  xTimerStart(timer_1khz, 0);
}

void loop() {
  // Serial.printf("%d %d\r\n", count, sw.getStatus());
  // buzzer.beep(driver::Buzzer::C4, 2000);
  // buzzer.beep(driver::Buzzer::D4, 2000);
  // buzzer.beep(driver::Buzzer::E4, 2000);
  // buzzer.beep(driver::Buzzer::F4, 2000);
  // buzzer.beep(driver::Buzzer::G4, 2000);
  // buzzer.beep(1000, 2000);
  /*
  Serial.print(vl53l0x.readRangeContinuousMillimeters());
  if (vl53l0x.timeoutOccurred()) {
    Serial.print(" TIMEOUT");
  }

  Serial.println();
  delay(50);
*/
}
