/**
 * @file state_machine.cpp
 * @brief
 */

#include "state_machine/state_machine.h"

#include "state_machine/state.h"

namespace state_machine {

RET StateMachine::changeState(state_t main, state_t sub, uint8_t is_printf) {
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
  next_state_ = state_list[main][sub];
  // 現在のステートをChangeStateに更新
  current_state_ = state_list[main_state::Idle][idle::sub_state::ChangeState];
  if (is_printf) printf("main = %3d, sub = %3d, name = %s\r\n", current_state_.main, current_state_.sub, current_state_.name);
  change_event_();
  return RET_OK;
}

}  // namespace state_machine
