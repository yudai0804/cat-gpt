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
  State previous_state_;
  State current_state_;
  State next_state_;
  size_t main_state_number_;
  std::vector<size_t> sub_state_number_;

public:
  StateMachine() {
    main_state_number_ = state_list.size();
    sub_state_number_.resize(main_state_number_);
    for (int i = 0; i < main_state_number_; i++) {
      sub_state_number_[i] = state_list[i].size();
    }
  }

  void init(State prev, State current, State next) {
    previous_state_ = prev;
    current_state_ = current;
    next_state_ = next;
  }

  virtual RET changeState(state_t main, state_t sub, uint8_t is_printf = 1) = 0;

  virtual RET requestChangeState(state_t main, state_t sub, uint8_t is_printf = 1) = 0;

  void onInterruptStateFunction() {
    current_state_.function();
  }

  State getPreviousState() { return previous_state_; }
  State getCurrentState() { return current_state_; }
};
}  // namespace state_machine
