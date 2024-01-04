/**
 * @file state_machine.h
 * @brief
 */

#pragma once
#include <stdint.h>
#include <stdio.h>

namespace state_machine {

/*
今回のシステムでは行わないが、大規模なシステムの場合
ステートが切り替わるタイミングで、様々なことを行う。
その場合、StateMachineクラスを継承して使うとよい。
今回は複雑なことは行わないため、StateMachineクラスをそのまま使用する。
*/

using state_t = uint8_t;

// 当初std::functionを使用する予定だったが、std::functionはメモリの動的確保が行われるらしいのでやめた。
// 今回はラムダ式等を使う予定はなかったため、関数ポインタで行うことにした。

struct State {
  state_t main;
  state_t sub;
  void (*function)(void);
};

constexpr State createState(state_t main, state_t sub, void (*func)(void)) {
  return {.main = main, .sub = sub, .function = func};
}

class StateMachine {
private:
  State previous_state_;
  State current_state_;

public:
  StateMachine(State prev, State current) {
    previous_state_ = prev;
    current_state_ = current;
  }

  void changeState(State next, uint8_t is_printf = 1) {
    previous_state_ = current_state_;
    current_state_ = next;
    if (is_printf) {
      printf("main = 0x%x, sub = 0x%x\r\n", current_state_.main,
             current_state_.sub);
    }
  }

  void process() {
    current_state_.function();
  }

  State getPreviousState() { return previous_state_; }
  State getCurrentState() { return current_state_; }
};
}  // namespace state_machine
