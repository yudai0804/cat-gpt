/**
 * @file wifi_tcp_client.h
 * @brief
 */

#pragma once

#include <stdint.h>
#include <stdio.h>

#include "Arduino.h"
#include "IPAddress.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiType.h"
#include "common/common.h"
#include "timer/timer_base.h"

namespace driver {

template <typename T>
class WifiTCPClient {
public:
  static constexpr timer::time_t TIMEOUT_MS = 5000;

private:
  const char *ssid_;
  const char *password_;
  const char *host_;
  const uint16_t port_;
  const IPAddress local_ip_;
  const IPAddress gateway_;
  const IPAddress subnet_;
  T timer_;

public:
  WifiTCPClient(const char *ssid, const char *password,
                const char *host, const uint16_t port,
                const IPAddress local_ip, const IPAddress gateway, const IPAddress subnet)
      : ssid_(ssid), password_(password), host_(host), port_(port), local_ip_(local_ip), gateway_(gateway), subnet_(subnet) {
  }

  RET init(bool enable_printf = 1) {
    if (!WiFi.config(local_ip_, gateway_, subnet_)) {
      if (enable_printf) printf("wifi config error\r\n");
      return RET_ERROR;
    }
    WiFi.begin(ssid_, password_);
    timer_.reset();
    while (WiFi.status() != WL_CONNECTED) {
      if (timer_.getElapsedTime() >= TIMEOUT_MS) {
        if (enable_printf) printf("wifi begin timeout\r\n");
        return RET_TIMEOUT_ERROR;
      }
    }
    if (enable_printf) printf("WiFi initalize success\r\n");
    return RET_OK;
  }

  RET transmit(uint8_t *data, size_t len, bool enable_printf = 1) {
    WiFiClient client;
    if (!client.connect(host_, port_, TIMEOUT_MS)) {
      if (enable_printf) printf("client connect error\r\n");
      return RET_ERROR;
    }
    client.write(data, len);
    client.stop();
    return RET_OK;
  }

  RET receive(uint8_t *data, size_t *len, bool enable_printf = 1) {
    WiFiClient client;
    if (!client.connect(host_, port_, TIMEOUT_MS)) {
      if (enable_printf) printf("client connect error\r\n");
      return RET_ERROR;
    }
    timer_.reset();
    while (client.available() == 0) {
      if (timer_.getElapsedTime() >= TIMEOUT_MS) {
        if (enable_printf) printf("receive timeout\r\n");
        client.stop();
        return RET_ERROR;
      }
    }
    size_t i = 0;
    while (client.available()) {
      data[i++] = client.read();
    }
    *len = i;
    client.stop();
    return RET_OK;
  }

  RET transmitAndReceive(uint8_t *transmit_data, size_t transmit_data_len,
                         uint8_t *receive_data, size_t *receive_data_len,
                         bool enable_printf = 1) {
    WiFiClient client;
    // transmit
    if (!client.connect(host_, port_, TIMEOUT_MS)) {
      if (enable_printf) printf("client connect error\r\n");
      return RET_ERROR;
    }
    client.write(transmit_data, transmit_data_len);
    // receive
    if (!client.connect(host_, port_, TIMEOUT_MS)) {
      if (enable_printf) printf("client connect error\r\n");
      return RET_ERROR;
    }
    timer_.reset();
    while (client.available() == 0) {
      if (timer_.getElapsedTime() >= TIMEOUT_MS) {
        if (enable_printf) printf("receive timeout\r\n");
        client.stop();
        return RET_ERROR;
      }
    }
    size_t i = 0;
    while (client.available()) {
      receive_data[i++] = client.read();
    }
    *receive_data_len = i;
    client.stop();
    return RET_OK;
  }

  const char *getSSID() { return ssid_; }
  const char *getPassword() { return password_; }
  const char *getHost() { return host_; }
  uint16_t getPort() { return port_; }
  IPAddress getLocalIP() { return local_ip_; }
  IPAddress getGateway() { return gateway_; }
  IPAddress getSubnet() { return subnet_; }
};

}  // namespace driver
