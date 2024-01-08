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
private:
  State previous_state_;
  State current_state_;
  size_t main_state_number_;
  std::vector<size_t> sub_state_number_;
  void (*change_event_)(void);

public:
  StateMachine(void (*change_event)(void), State prev, State current) {
    change_event_ = change_event;
    previous_state_ = prev;
    current_state_ = current;
    main_state_number_ = state_list.size();
    sub_state_number_.resize(main_state_number_);
    for (int i = 0; i < main_state_number_; i++) {
      sub_state_number_[i] = state_list[i].size();
    }
  }

  RET changeState(state_t main, state_t sub, uint8_t is_printf = 1) {
    // 引数が正常かチェック
    if (main >= main_state_number_) {
      if (is_printf) printf("main state argument error\r\nmain = %3d, sub = %3d\r\n", main, sub);
      return RET_ARGUMENT_ERROR;
    }
    if (sub >= sub_state_number_[main]) {
      if (is_printf) printf("sub state argument error\r\nmain = %3d, sub = %3d\r\n", main, sub);
      return RET_ARGUMENT_ERROR;
    }
    // ステートを更新
    previous_state_ = current_state_;
    current_state_ = state_list[main][sub];
    if (is_printf) printf("main = %3d, sub = %3d, name = %s\r\n", current_state_.main, current_state_.sub, current_state_.name);
    change_event_();
    return RET_OK;
  }

  void process() {
    current_state_.function();
  }

  State getPreviousState() { return previous_state_; }
  State getCurrentState() { return current_state_; }
};
}  // namespace state_machine
