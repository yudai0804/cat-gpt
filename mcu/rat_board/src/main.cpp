#include <Arduino.h>

#include "driver/buzzer.h"
#include "driver/drv8835.h"
#include "driver/led.h"
#include "driver/switch.h"
#include "driver/wrapper/wifi_tcp_client.h"
#include "freertos/FreeRTOS.h"
#include "private_information.h"
#include "rat_communication.h"
#include "rat_hardware.h"
#include "rat_information.h"
#include "state_machine/state.h"
#include "state_machine/state_machine.h"
#include "timer/timer.h"
#include "timer/timer_1ms.h"
#include "timer/timer_20ms.h"
#include "timer/timer_base.h"

TimerHandle_t timer_1ms;
TimerHandle_t timer_20ms;

driver::WifiTCPClient wifi_client(timer::USE_TIMER_1MS, SSID, PASSWORD, HOST, PORT, LOCAL_IP, GATEWAY, SUBNET);

rat::Hardware rat_hardware;
rat::Information rat_information;
// タイマーは仮
rat::Communication rat_communication{&wifi_client, &rat_information, timer::USE_TIMER_1MS};

void timer1msHandler(void *param) {
  timer::Timer1ms_update();
  rat_hardware.onInterrupt1ms();
}

void timer20msHandler(void *param) {
  timer::Timer20ms_update();
  // 使わないのでコメントアウト
  // rat_hardware.onInterruptForToF();
  // rat_communication.onInterruptStateFunction();
}

/**
 * @brief WiFi用タイマー。実行時間にかかる時間が分からないため、タイマーではなく、タイマーもどきを使用する
 */
void timer50msProcess(void) {
  static timer::Timer1ms interval_timer;
  if (interval_timer.getElapsedTime() < 50) return;
  // 使わないのでコメントアウト
  // rat_communication.communicate();
  interval_timer.reset();
}

// #if 0
void controlMotorByKeyboard() {
  auto &drv8835 = rat_hardware.drv8835_;
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
// #endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  rat_hardware.init();
  // wifi_client.init();

  timer_1ms = xTimerCreate("TIM_1MS", 1, pdTRUE, NULL, timer1msHandler);
  timer_20ms = xTimerCreate("TIM_20MS", 20, pdTRUE, NULL, timer20msHandler);
  rat_hardware.led_red_.blinkByFrequency(1);
  rat_hardware.led_white_.blinkByFrequency(1);
  xTimerStart(timer_1ms, 0);
  xTimerStart(timer_20ms, 0);
}

int count = 0;

void loop() {
  // timer50msProcess();
#if 0
  char tx_data[128];
  char rx_data[128];
  size_t receive_data_len;
  sprintf(tx_data, "%d\r\n", ++count);
  wifi_client.transmitAndReceive((uint8_t *)tx_data, strlen(tx_data), (uint8_t *)rx_data, &receive_data_len);
  printf("%s\r\n", rx_data);
#endif
  controlMotorByKeyboard();
  // Serial.printf("time = %d, red = %d, white = %d\r\n", timer::Timer1ms_getCurrentTime(), led_red.getStatus(), led_white.getStatus());
  // Serial.printf("time = %d, status = %d\r\n", limit_switch.getTime(), limit_switch.getStatus());
}
