/**
 * @file state_machine.h
 * @brief
 */

#pragma once
#include <stdint.h>
#include <stdio.h>

#include <functional>

namespace state_machine {

using state_t = uint8_t;

struct State {
  state_t main;
  state_t sub;
  std::function<void(void)> function;
};

class StateMachine {
private:
  State previous_state_;
  State current_state_;

  State getIdleState() {
    State ret_state{
        .main = static_cast<state_t>(MainState::Idle),
        .sub = static_cast<state_t>(IdleSubState::Idle),
        .function = [] {
          printf("called idle function.\r\n");
        }};
    return ret_state;
  }

public:
  StateMachine() {
    previous_state_ = current_state_ = getIdleState();
  }

  void setCurrentState(State current) {
    current_state_ = current;
  }

  void changeState(State next, uint8_t is_printf = 1) {
    previous_state_ = current_state_;
    current_state_ = next;
    if (is_printf) {
      printf("main = %d, sub = %d\r\n", current_state_.main,
             current_state_.sub);
    }
    current_state_.function();
  }

  State getPreviousState() { return previous_state_; }
  State getCurrentState() { return current_state_; }
};
}  // namespace state_machine
