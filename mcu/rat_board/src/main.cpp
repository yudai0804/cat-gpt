#include <Arduino.h>

#include "communication/communication.h"
#include "communication/information.h"
#include "driver/buzzer.h"
#include "driver/drv8835.h"
#include "driver/led.h"
#include "driver/switch.h"
#include "driver/wrapper/wifi_tcp_client.h"
#include "freertos/FreeRTOS.h"
#include "private_information.h"
#include "rat.h"
#include "state_machine/state.h"
#include "state_machine/state_machine.h"
#include "timer/timer.h"
#include "timer/timer_1ms.h"
#include "timer/timer_20ms.h"
#include "timer/timer_base.h"

TimerHandle_t timer_1ms;
TimerHandle_t timer_20ms;
TaskHandle_t state_machine_task_handle;

driver::WifiTCPClient wifi_client(timer::USE_TIMER_1MS, SSID, PASSWORD, HOST, PORT, RAT_IP, GATEWAY, SUBNET);
communication::Information information;
communication::Communication com{&wifi_client, &information, timer::USE_TIMER_1MS};
Hardware hardware;
bool is_initialize_end = false;
// タイマーは仮
timer::Timer1ms test_timer;
float test_target;
void timer1msHandler(void *param) {
  timer::Timer1ms_update();
  if (!is_initialize_end) return;
  hardware.onInterrupt1ms();
}

void timer20msHandler(void *param) {
  if (!is_initialize_end) return;
  timer::Timer20ms_update();

  state_machine::other::on_interrupt_process();

  //   printf("limit sw = %d\r\n", hardware.getLimitSwitch());
  /*
  float target;
  if (test_timer.getElapsedTime() < 2000) {
    target = -0.4f;
  } else {
    target = 0;
  }
  */
  // 一旦コメントアウト
  // hardware.runByVelocity(target, 0);

  // 使わないのでコメントアウト
  // hardware.onInterruptForToF();
}

/**
 * @brief WiFi用タイマー。実行時間にかかる時間が分からないため、タイマーではなく、タイマーもどきを使用する
 */
void timer50msProcess(void) {
  static timer::Timer1ms interval_timer;
  if (interval_timer.getElapsedTime() < 50) return;
  interval_timer.reset();
  com.communicate();
}

/**
 * 状態遷移用タスク
 */
void stateMachineTask(void *param) {
  while (1) {
    com.onInterruptStateFunction();
  }
}

// #if 0
void controlMotorByKeyboard() {
  auto &drv8835 = hardware.drv8835_;
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
  printf("program start\r\n");
  // タイマー関連を初期化
  timer_1ms = xTimerCreate("TIM_1MS", 1, pdTRUE, NULL, timer1msHandler);
  timer_20ms = xTimerCreate("TIM_20MS", 20, pdTRUE, NULL, timer20msHandler);
  // hardware.led_red_.blinkByFrequency(1);
  // hardware.led_white_.blinkByFrequency(1);
  xTimerStart(timer_1ms, 0);
  xTimerStart(timer_20ms, 0);
  // ペリフェラル関連を初期化
  hardware.init();
  int ret = wifi_client.init();
  printf("wifi status = %d\r\n", ret);

  is_initialize_end = true;
  // 状態遷移タスクの開始
  xTaskCreatePinnedToCore(stateMachineTask, "STATE_MACHINE_TASK", 4096, NULL, 1, NULL, APP_CPU_NUM);

  test_timer.reset();
}

int count = 0;

void loop() {
  timer50msProcess();
#if 0
  char tx_data[128];
  char rx_data[128];
  size_t receive_data_len;
  sprintf(tx_data, "%d\r\n", ++count);
  wifi_client.transmitAndReceive((uint8_t *)tx_data, strlen(tx_data), (uint8_t *)rx_data, &receive_data_len);
  printf("%s\r\n", rx_data);
#endif
  // controlMotorByKeyboard();
  //  Serial.printf("time = %d, red = %d, white = %d\r\n", timer::Timer1ms_getCurrentTime(), led_red.getStatus(), led_white.getStatus());
  //  Serial.printf("time = %d, status = %d\r\n", limit_switch.getTime(), limit_switch.getStatus());
}
