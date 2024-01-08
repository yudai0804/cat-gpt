#include <Arduino.h>

#include "driver/buzzer.h"
#include "driver/drv8835.h"
#include "driver/led.h"
#include "driver/switch.h"
#include "driver/wrapper/wifi_tcp_client.h"
#include "freertos/FreeRTOS.h"
#include "private_information.h"
#include "state_machine/state.h"
#include "state_machine/state_machine.h"
#include "timer/timer.h"
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
driver::LED led_red(timer::USE_TIMER_1MS, 12);
driver::LED led_white(timer::USE_TIMER_1MS, 14);
driver::Switch limit_switch(timer::USE_TIMER_1MS, 15, 1);

driver::WifiTCPClient wifi_client(timer::USE_TIMER_1MS, SSID, PASSWORD, HOST, PORT, LOCAL_IP, GATEWAY, SUBNET);

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
  wifi_client.init();

  timer_1ms = xTimerCreate("TIM_1MS", 1, pdTRUE, NULL, timer1msHandler);
  led_red.blinkByInterval(500);
  led_white.blinkByFrequency(1);
  xTimerStart(timer_1ms, 0);
  // buzzer.beep(driver::Buzzer::F4, 1000);
}

int count = 0;

void loop() {
  char tx_data[128];
  char rx_data[128];
  size_t receive_data_len;
  sprintf(tx_data, "%d\r\n", ++count);
  wifi_client.transmitAndReceive((uint8_t *)tx_data, strlen(tx_data), (uint8_t *)rx_data, &receive_data_len);
  printf("%s\r\n", rx_data);
  // controlMotorByKeyboard();
  // Serial.printf("time = %d, red = %d, white = %d\r\n", timer::Timer1ms_getCurrentTime(), led_red.getStatus(), led_white.getStatus());
  // Serial.printf("time = %d, status = %d\r\n", limit_switch.getTime(), limit_switch.getStatus());
}
