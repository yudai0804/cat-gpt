/**
 * @file state.h
 * @brief
 */

#pragma once

#include "state_machine/state_machine.h"

namespace state_machine {

namespace main_state {

enum Name : state_t {
  Idle = 0x0,
  Search,
  Suspend,
  CaughtByCat,
  Feed,
  Error = 0x8,
};

}  // namespace main_state

// 万が一違うステートを代入してしまっても大丈夫なように、同じ値のステートは存在しないようになっている。
namespace idle {

namespace sub_state {
enum Name : state_t {
  Idle = 0x00,
};
}  // namespace sub_state

void idle_process();

}  // namespace idle

namespace search {

namespace sub_state {
enum Name : state_t {
  Start = 0x10,
  Search,
  DetectObstacle,
  AvoidObstacle,
  AppelToCat,
  Finish,
};
}

void start_process();
void search_process();
void detect_obstacle_process();
void avoid_obstacle_process();
void appel_to_cat_process();
void finish_process();

}  // namespace search

namespace caught_by_cat {

namespace sub_state {
enum Name : state_t {
  Start = 0x20,
  Caught,
  Finish,
};
}

void start_process();
void caught_process();
void finish_process();

}  // namespace caught_by_cat

namespace feed {

namespace sub_state {
enum Name : state_t {
  Start = 0x30,
  AppelToCat,
  Feed,
  Finish,
};
}

void start_process();
// 使用するのは給餌器側のみ
void appel_to_cat_process();
void feed_process();
void finish_process();

}  // namespace feed

namespace error {

namespace sub_state {
enum Name : state_t {
  CanNotMove = 0x80,
  CommunicationFailed
};
}

void can_not_move_process();
void communication_failed_process();

}  // namespace error

}  // namespace state_machine
