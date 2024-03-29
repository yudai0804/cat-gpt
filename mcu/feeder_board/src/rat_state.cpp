/**
 * @file feeder_state.cpp
 * @brief
 */

#include <stdio.h>

#include "rat.h"
#include "state_machine/state.h"
#include "timer/timer_1ms.h"

namespace state_machine {
// clang-format off
std::vector<std::vector<State>> state_list = {
  {
    {.main = main_state::Idle, .sub = idle::sub_state::Idle, .name = "Idle::Idle", .function = idle::idle_process},
    {.main = main_state::Idle, .sub = idle::sub_state::NoConnect, .name = "Idle::NoConnect", .function = idle::no_connect_process},
    {.main = main_state::Idle, .sub = idle::sub_state::ChangingState, .name = "Idle::ChangingState", .function = idle::changing_state_process},
  },
  {
    {.main = main_state::Search, .sub = search::sub_state::Start, .name = "Search::Start", .function = search::start_process},
    {.main = main_state::Search, .sub = search::sub_state::Search, .name = "Search::Search", .function = search::search_process},
    {.main = main_state::Search, .sub = search::sub_state::DetectObstacle, .name = "Search::DetectObstacle", .function = search::detect_obstacle_process},
    {.main = main_state::Search, .sub = search::sub_state::AvoidObstacle, .name = "Search::AvoidObstacle", .function = search::avoid_obstacle_process},
    {.main = main_state::Search, .sub = search::sub_state::AppelToCat, .name = "Search::AppelToCat", .function = search::appel_to_cat_process},
    {.main = main_state::Search, .sub = search::sub_state::Finish, .name = "Search::Finish", .function = search::finish_process},
  },
  {
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Start, .name = "CaughtByCat::Start", .function = caught_by_cat::start_process},
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Caught, .name = "CaughtByCat::Caught", .function = caught_by_cat::caught_process},
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Finish, .name = "CaughtByCat::Finish", .function = caught_by_cat::finish_process},
  },
  {
    {.main = main_state::Feed, .sub = feed::sub_state::Start, .name = "Feed::Start", .function = feed::start_process},
    {.main = main_state::Feed, .sub = feed::sub_state::AppelToCat, .name = "Feed::AppelToCat", .function = feed::appel_to_cat_process},
    {.main = main_state::Feed, .sub = feed::sub_state::Feed, .name = "Feed::Feed", .function = feed::feed_process},
    {.main = main_state::Feed, .sub = feed::sub_state::Finish, .name = "Feed::Start", .function = feed::finish_process},
  },
  {
    {.main = main_state::Manual, .sub = manual::sub_state::Manual, .name = "Manual::Manual", .function = manual::manual_process}
  },
  {
    {.main = main_state::Error, .sub = error::sub_state::Error, .name = "Error::Error", .function = error::error_process}
  }
};
// clang-format on

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
}

void search_process() {
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
  if (information.getIsStartManualFeed()) {
    hardware.led_red_.blinkByFrequency(1);
    hardware.drv8835_.outputA(0.5f);
    printf("manual feed start, quantity = %f\r\n", information.getManualFeed());
    uint32_t delay = (uint32_t)(information.getManualFeed() * 1000);
    vTaskDelay(delay);
    // clear
    information.setIsStartManualFeed(false);
    hardware.led_red_.blinkByFrequency(0);
    hardware.drv8835_.outputA(0.5f);
  } else {
    hardware.drv8835_.outputA(0);
    hardware.led_red_.blinkByFrequency(0);
  }
}
}  // namespace manual

namespace error {

void error_process() {
}

}  // namespace error

}  // namespace state_machine
