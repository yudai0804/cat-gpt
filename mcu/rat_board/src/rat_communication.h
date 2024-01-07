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
#include "timer/timer.h"

namespace rat {

class Communication : public driver::WifiTCPClient {
private:
  state_machine::StateMachine *state_machine_;
  Information *information_;
  static constexpr size_t BUFFER_SIZE = 64;

public:
  Communication(timer::UseTimer use_timer,
                const char *ssid, const char *password,
                const char *host, const uint16_t port,
                const IPAddress local_ip, const IPAddress gateway, const IPAddress subnet,
                state_machine::StateMachine *_state_machine, Information *information)
      : state_machine_(_state_machine), information_(information), driver::WifiTCPClient(use_timer, ssid, password, host, port, local_ip, gateway, subnet) {}
};
}  // namespace rat
