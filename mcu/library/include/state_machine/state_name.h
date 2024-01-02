/**
 * @file state_name.h
 * @brief
 */

#pragma once

#include "state_machine/state_machine.h"

namespace state_machine {

enum class MainState : state_t {
  None,
  Serch,
  Suspend,
  CaughtByCat,
  Feed,
  Error,
};

enum class SerchSubState : state_t {
  Start,
  Serching,
  DetectObstacle,
  AvoidObstacle,
  AppealToCat,
  Finish,
};

enum class SuspendSubState : state_t {
  Start,
  Suspend,
  WakeUp,
  Finish,
};

enum class CaughtByACatSubState : state_t {
  Start,
  Caught,
  Finish,
};

enum class FeedSubState : state_t {
  Start,
  AppealForFeeding,
  Feeding,
  Finish,
};

enum class ErrorSubState : state_t {
  CanNotMove,
  Disconnect,
};

} // namespace state_machine
