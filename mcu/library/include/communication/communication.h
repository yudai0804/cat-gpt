/**
 * @file communication.h
 * @brief
 */

#pragma once

#include <stdint.h>
#include <string.h>

#include "Arduino.h"
#include "common/buffer.h"
#include "common/common.h"
#include "communication/information.h"
#include "driver/wrapper/wifi_tcp_client.h"
#include "state_machine/state.h"
#include "state_machine/state_machine.h"
#include "timer/timer.h"
#include "timer/timer_base.h"

namespace communication {

using namespace state_machine;

class Communication : public state_machine::StateMachine {
public:
  static constexpr size_t BUFFER_SIZE = 64;
  enum Header : uint8_t {
    StateInformation,
    ChangeState,
    RequestChangeState,
    SetSearchMode,
    SetAppealMode,
    SetFoodQuantity,
    ManualMove,
    ManualFeed,
    ACK = 0x80,
  };

private:
  driver::WifiTCPClient *client_;
  Information information_;
  common::Buffer<uint8_t> buffer_{BUFFER_SIZE};
  timer::TimerBase *timer_;
  State DEFAULT_STATE;

  void convertFloatToUint8(uint8_t *uint8_data, float *float_data) {
    uint8_t data[4];
    uint8_t *pointer = &data[0];
    pointer = (uint8_t *)float_data;
    for (int i = 0; i < 4; i++)
      uint8_data[i] = pointer[i];
  }

  void convertUint8ToFloat(uint8_t *uint8_data, float *float_data) {
    uint8_t *pointer = (uint8_t *)float_data;
    for (int i = 0; i < 4; i++)
      pointer[i] = uint8_data[i];
  }

  void receive(uint8_t header, uint8_t *receive_data, size_t length) {
    switch ((uint8_t)header) {
      case ChangeState: {
        constexpr uint8_t LENGTH = 3;
        uint8_t transmit_data[LENGTH];
        RET ret = changeState(receive_data[0], receive_data[1]);
        State state = getCurrentState();
        transmit_data[0] = (ret == RET_OK) ? 1 : 0;
        transmit_data[1] = state.main;
        transmit_data[2] = state.sub;
        setOrder(ChangeState + ACK, transmit_data, LENGTH);
      } break;
      case RequestChangeState + ACK: {
        uint8_t is_success = receive_data[0];
        if (is_success != 1) printf("RequestChangeState Failed\r\n");
        changeState(receive_data[1], receive_data[2]);
      } break;
      case SetSearchMode: {
        constexpr uint8_t LENGTH = 2;
        uint8_t transmsit_data[LENGTH];
        RET ret = information_.setSearchMode(receive_data[0]);
        auto mode = information_.getSearchMode();
        transmsit_data[0] = (ret == RET_OK) ? 1 : 0;
        transmsit_data[1] = mode;
        setOrder(SetSearchMode + ACK, transmsit_data, LENGTH);

      } break;
      case SetAppealMode: {
        constexpr uint8_t LENGTH = 2;
        uint8_t transmsit_data[LENGTH];
        RET ret = information_.setAppealMode(receive_data[0]);
        auto mode = information_.getAppealMode();
        transmsit_data[0] = (ret == RET_OK) ? 1 : 0;
        transmsit_data[1] = mode;
        setOrder(SetAppealMode + ACK, transmsit_data, LENGTH);
      } break;
      case ManualMove: {
        constexpr uint8_t LENGTH = 9;
        uint8_t transmsit_data[LENGTH];
        float manual_velocity, manual_omega;
        convertUint8ToFloat(&receive_data[0], &manual_velocity);
        convertUint8ToFloat(&receive_data[0], &manual_omega);
        RET ret1 = information_.setManualVelocity(manual_velocity);
        RET ret2 = information_.setManualOmega(manual_omega);
        manual_velocity = information_.getManualVelocity();
        manual_omega = information_.getManualOmega();
        transmsit_data[0] = (ret1 == RET_OK && ret2 == RET_OK) ? 1 : 0;
        convertFloatToUint8(&transmsit_data[1], &manual_velocity);
        convertFloatToUint8(&transmsit_data[5], &manual_omega);
        setOrder(ManualMove + ACK, transmsit_data, LENGTH);
      } break;
    }
  }

  void decode(uint8_t *receive_buffer, size_t length) {
    // 受信時の処理
    if (length == 0) return;
    uint8_t order_count = receive_buffer[0];
    Header header;
    uint8_t len;
    constexpr uint8_t ORDER_OFFSET = 1;
    size_t i = ORDER_OFFSET;
    while (order_count) {
      header = (Header)receive_buffer[i++];
      len = receive_buffer[i++];
      receive(header, &receive_buffer[i], length);
      i += len;
      order_count--;
    }
  }

  void setOrder(uint8_t header, uint8_t *data, uint8_t length) {
    // orderをインクリメントする
    buffer_.buffer_[0]++;
    // buffer_に命令を追加
    buffer_.addBuffer((uint8_t)header);
    buffer_.addBuffer(data, length);
  }

  void transmitStateInformation() {
    constexpr uint8_t LENGTH = 2;
    uint8_t data[LENGTH];
    data[0] = (uint8_t)current_state_.main;
    data[1] = (uint8_t)current_state_.sub;
    setOrder(Header::StateInformation, data, LENGTH);
  }

  void transmitRequestChangeState(state_t main, state_t sub) {
    constexpr uint8_t LENGTH = 2;
    uint8_t data[LENGTH];
    data[0] = (uint8_t)current_state_.main;
    data[1] = (uint8_t)current_state_.sub;
    setOrder(Header::RequestChangeState, data, LENGTH);
  }

public:
  Communication(driver::WifiTCPClient *client, timer::UseTimer use_timer)
      : client_(client), StateMachine() {
    timer_ = timer::createTimer(use_timer);
  }

  void communicate() {
    // 送受信処理と受信時の各変数に代入処理
    uint8_t transmit_buffer[BUFFER_SIZE];
    size_t transmit_buffer_size;
    uint8_t receive_buffer[BUFFER_SIZE];
    size_t receive_buffer_length;
    RET ret;
    // queueに現在のステートを追加
    transmitStateInformation();

    // bufferをコピー
    // bufferをポインタではなく、コピーする理由はWifiの送受信中に、setOrderが発生して、bufferが書き換えられる可能性があるため。
    transmit_buffer_size = buffer_.copyBuffer(transmit_buffer);
    buffer_.clear();
    ret = client_->transmitAndReceive(transmit_buffer, transmit_buffer_size, receive_buffer, &receive_buffer_length);
    if (ret != RET_OK) {
      // 通信に失敗した場合はNoConnectに移動
      changeState(main_state::Idle, idle::sub_state::NoConnect);
      return;
    }

    decode(receive_buffer, receive_buffer_length);
  }
  /**
   * @brief ステートを移行する
   * @details 現在のステートと移行したいステートが同じだった場合はステートの遷移は行われない
   * 値が範囲外だった場合も状態の遷移は行われない
   * @param main
   * @param sub
   * @return
   */
  RET changeState(state_t main, state_t sub) {
    // 引数が正常かチェック
    if (main >= main_state_number_) {
      printf("main state argument error\r\nmain = %3d, sub = %3d\r\n", main, sub);
      return RET_ARGUMENT_ERROR;
    }
    if (sub >= sub_state_number_[main]) {
      printf("sub state argument error\r\nmain = %3d, sub = %3d\r\n", main, sub);
      return RET_ARGUMENT_ERROR;
    }
    // ステートを更新
    // ステートが変化していなかった場合はその後の処理は行わない
    bool is_changed_state = (main != current_state_.main || sub != current_state_.sub);
    if (!is_changed_state) return RET_ERROR;

    current_state_ = state_list[main][sub];
    printf("main = %3d, sub = %3d, name = %s\r\n", current_state_.main, current_state_.sub, current_state_.name);
    // 現在のステートを送信
    transmitStateInformation();
    return RET_OK;
  }

  /**
   * @brief ステートの移行をサーバー側にrequestする
   * @details サーバーからのレスポンスが来るまではChangingStateで待機する
   * 現在のステートと移行したいステートが同じだった場合はステートの遷移は行われない
   * 値が範囲外だった場合も状態の遷移は行われない
   *
   * @param main
   * @param sub
   * @return
   */
  RET requestChangeState(state_t main, state_t sub) {
    // idleステートに設定
    RET ret = changeState(main_state::Idle, idle::sub_state::ChangingState);
    if (ret != RET_OK) return ret;
    // requestを送信
    transmitRequestChangeState(main, sub);
    return RET_OK;
  }

  const Information &getInformation() { return information_; }
};
}  // namespace communication
