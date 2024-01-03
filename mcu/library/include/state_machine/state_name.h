/**
 * @file state_name.h
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
namespace sub_state {

namespace idle {
enum Name : state_t {
  Idle = 0x00,
};
}  // namespace idle

namespace search {
enum Name : state_t {
  Start = 0x10,
  Search,
  DetectObstacle,
  AvoidObstacle,
  AppelToCat,
  Finish,
};

}  // namespace search

namespace caught_by_cat {
enum Name : state_t {
  Start = 0x20,
  Caught,
  Finish,
};

}  // namespace caught_by_cat

namespace feed {
enum Name : state_t {
  Start = 0x30,
  AppelToCat,
  Feed,
  Finish,
};
}  // namespace feed

namespace error {
enum Name : state_t {
  CanNotMove = 0x80,
  CommunicationFailed
};
}  // namespace error

}  // namespace sub_state

}  // namespace state_machine
