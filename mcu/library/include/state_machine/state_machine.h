/**
 * @file state_machine.h
 * @brief
 */

#pragma once
#include <stdint.h>
#include <stdio.h>

#include <functional>

#include "state_machine/state.h"

namespace state_machine {

/*
今回のシステムでは行わないが、大規模なシステムの場合
ステートが切り替わるタイミングで、様々なことを行う。
その場合、StateMachineクラスを継承して使うとよい。
今回は複雑なことは行わないため、StateMachineクラスをそのまま使用する。
*/

class StateMachine {
private:
  State previous_state_;
  State current_state_;

  State getIdleState() {
    State ret_state{
        .main = main_state::Idle,
        .sub = idle::sub_state::Idle,
        .function = [] {
          printf("called idle function.\r\n");
        }};
    return ret_state;
  }

public:
  StateMachine() {
    previous_state_ = current_state_ = getIdleState();
  }

  void changeState(State next, uint8_t is_printf = 1) {
    previous_state_ = current_state_;
    current_state_ = next;
    if (is_printf) {
      printf("main = 0x%x, sub = 0x%x\r\n", current_state_.main,
             current_state_.sub);
    }
  }

  void changeState(state_t next_main, state_t next_sub,
                   std::function<void(void)> next_func, uint8_t is_printf = 1) {
    State tmp = {.main = next_main, .sub = next_sub, .function = next_func};
    changeState(tmp, is_printf);
  }

  void process() {
    current_state_.function();
  }

  State getPreviousState() { return previous_state_; }
  State getCurrentState() { return current_state_; }
};
}  // namespace state_machine
