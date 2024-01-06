/**
 * @file rat_communication.h
 * @brief
 */

#pragma once

#include <stdint.h>
#include <string.h>

#include "common.h"
#include "driver/wrapper/wifi_tcp_client.h"
#include "rat_information.h"
#include "state_machine/state_machine.h"

namespace rat {

template <typename T>
class Communication : public driver::WifiTCPClient<T> {
private:
  state_machine::StateMachine *state_machine_;
  Information *information_;
  static constexpr size_t BUFFER_SIZE = 64;

public:
  Communication(const char *ssid, const char *password,
                const char *host, const uint16_t port,
                const IPAddress local_ip, const IPAddress gateway, const IPAddress subnet,
                state_machine::StateMachine *_state_machine, Information *information)
      : state_machine_(_state_machine), information_(information), driver::WifiTCPClient<T>(ssid, password, host, port, local_ip, gateway, subnet) {}

  RET transmitStartData() {
    char transmit_buffer[BUFFER_SIZE];
    size_t receive_size;
    char receive_buffer[BUFFER_SIZE];
    // 受信バッファはダミー
    return this->transmitAndReceive((uint8_t *)transmit_buffer, strlen(transmit_buffer), receive_buffer, &receive_size);
  }

  RET transmitAlive() {
    char transmit_buffer[BUFFER_SIZE];
    char receive_buffer[BUFFER_SIZE];
    size_t receive_size;
    sprintf(transmit_buffer, "current time = %d[ms]\r\n", this->timer_.getCurrentTime());
    return this->transmitAndReceive(transmit_buffer, strlen(transmit_buffer), receive_buffer, &receive_size);
  }

  RET transmitRatInformation() {
    // TODO: 後日実装する
    return RET_OK;
  }
};
}  // namespace rat
