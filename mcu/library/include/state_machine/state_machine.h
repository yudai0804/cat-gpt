/**
 * @file state_machine.h
 * @brief
 */

#pragma once
#include <stdint.h>
#include <stdio.h>

#include <cstddef>
#include <vector>

#include "common/common.h"

namespace state_machine {

using state_t = uint8_t;

// 当初std::functionを使用する予定だったが、std::functionはメモリの動的確保が行われるらしいのでやめた。
// 今回はラムダ式等を使う予定はなかったため、関数ポインタで行うことにした。

struct State {
  state_t main;
  state_t sub;
  const char* name;
  void (*function)(void);
};

extern std::vector<std::vector<State>> state_list;

class StateMachine {
protected:
  State next_state_;
  State current_state_;
  State previous_state_;
  size_t main_state_number_;
  std::vector<size_t> sub_state_number_;

public:
  StateMachine();

  virtual RET changeState(state_t main, state_t sub) = 0;
  virtual RET requestChangeState(state_t main, state_t sub) = 0;

  void onInterruptStateFunction() {
    current_state_.function();
  }

  State getCurrentState() { return current_state_; }
  State getNextState() { return next_state_; }
  State getPreviousState() { return previous_state_; }
};
}  // namespace state_machine
