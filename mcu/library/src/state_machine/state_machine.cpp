/**
 * @file state_machine.cpp
 * @brief
 */

#include "state_machine/state_machine.h"

#include "state_machine/state.h"

namespace state_machine {
StateMachine::StateMachine() {
  main_state_number_ = state_list.size();
  sub_state_number_.resize(main_state_number_);
  for (int i = 0; i < main_state_number_; i++) {
    sub_state_number_[i] = state_list[i].size();
  }
  // 初期ステートを設定
  current_state_ = state_list[main_state::Idle][idle::sub_state::NoConnect];
}

}  // namespace state_machine
