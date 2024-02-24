/**
 * @file feeder_process.cpp
 * @brief
 */

#include <stdio.h>

#include "feeder.h"
#include "state_machine/state.h"
#include "timer/timer_1ms.h"

namespace state_machine {

namespace other {

void change_state_process() {
  // Manualからの遷移を検出した場合はモーターを停止する
  auto current = com.getCurrentState();
  auto previous = com.getPreviousState();

  if (previous.main == main_state::Manual && previous.sub == manual::sub_state::Manual) {
    hardware.stop();
  }
  // モーターを動かす必要がない時はモーターを止める(Idle::ChangingState含む)
  auto is_idle = (current.main == main_state::Idle);
  auto is_error = (current.main == main_state::Error);

  if (is_idle || is_error) {
    hardware.stop();
  }
}

void on_interrupt_process() {
}

}  // namespace other

namespace idle {

void idle_process() {
  hardware.led_white_.blinkByFrequency(5);
  hardware.led_red_.blinkByFrequency(5);
  vTaskDelay(3000);
  // com.requestChangeState(main_state::Search, search::sub_state::Start);
}

void no_connect_process() {
  hardware.led_white_.blinkByFrequency(5);
  hardware.led_red_.blinkByFrequency(0);
}
void changing_state_process() {
}

}  // namespace idle

namespace search {

void start_process() {
  vTaskDelay(3000);
  // com.requestChangeState(main_state::Search, search::sub_state::Search);
}

void search_process() {
  vTaskDelay(3000);
  // com.requestChangeState(main_state::Search, search::sub_state::Start);
}

void detect_obstacle_process() {
}

void avoid_obstacle_process() {
}

void appel_to_cat_process() {
}

void finish_process() {
}

}  // namespace search

namespace caught_by_cat {

void start_process() {
}

void caught_process() {
}

void finish_process() {
}

}  // namespace caught_by_cat

namespace feed {

void start_process() {
}

// 使用するのは給餌器側のみ
void appel_to_cat_process() {
}

void feed_process() {
}

void finish_process() {
}

}  // namespace feed

namespace manual {

void manual_process() {
  // TODO: 実装する
}
}  // namespace manual

namespace error {

void error_process() {
}

}  // namespace error

}  // namespace state_machine
