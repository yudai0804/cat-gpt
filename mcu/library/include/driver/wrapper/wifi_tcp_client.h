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
#include "timer/timer.h"
#include "timer/timer_base.h"

namespace driver {

class WifiTCPClient {
public:
  static constexpr timer::time_t TIMEOUT_MS = 1000;

private:
  const char *ssid_;
  const char *password_;
  const char *host_;
  const uint16_t port_;
  const IPAddress local_ip_;
  const IPAddress gateway_;
  const IPAddress subnet_;
  timer::TimerBase *timer_;
  bool is_connected_ = false;

public:
  WifiTCPClient(timer::UseTimer use_timer,
                const char *ssid, const char *password,
                const char *host, const uint16_t port,
                const IPAddress local_ip, const IPAddress gateway, const IPAddress subnet)
      : ssid_(ssid), password_(password), host_(host), port_(port), local_ip_(local_ip), gateway_(gateway), subnet_(subnet) {
    timer_ = timer::createTimer(use_timer);
  }

  RET init() {
    if (!WiFi.config(local_ip_, gateway_, subnet_)) {
      printf("wifi config error\r\n");
      return RET_ERROR;
    }
    WiFi.begin(ssid_, password_);
    timer_->reset();
    while (WiFi.status() != WL_CONNECTED) {
      if (timer_->getElapsedTime() >= TIMEOUT_MS) {
        printf("wifi begin timeout\r\n");
        return RET_TIMEOUT_ERROR;
      }
    }
    printf("WiFi initalize success\r\n");
    return RET_OK;
  }

  RET transmit(uint8_t *data, size_t len, bool is_stop = 1) {
    WiFiClient client;
    // 通信確立
    if (!client.connect(host_, port_, TIMEOUT_MS)) {
      // 何回も失敗のログが出るとうざいので、1回だけ出るようにする
      if (is_connected_)
        printf("wifi connect error\r\n");
      is_connected_ = false;
      return RET_ERROR;
    }
    // 送信
    client.write(data, len);
    // 通信終了
    if (is_stop) client.stop();
    is_connected_ = true;
    return RET_OK;
  }

  /**
   * @brief
   *
   * @param data
   * @param len 受信したデータ長さ
   * @return
   */
  RET receive(uint8_t *data, size_t *len, bool is_stop = 1) {
    WiFiClient client;
    // 通信確立
    if (!client.connect(host_, port_, TIMEOUT_MS)) {
      // 何回も失敗のログが出るとうざいので、1回だけ出るようにする
      if (is_connected_)
        printf("wifi connect error\r\n");
      is_connected_ = false;
      return RET_ERROR;
    }
    timer_->reset();
    // データを受信するまで待機。受信に一定時間以上かかった場合はタイムアウト
    while (client.available() == 0) {
      if (timer_->getElapsedTime() >= TIMEOUT_MS) {
        // 何回も失敗のログが出るとうざいので、1回だけ出るようにする
        if (is_connected_)
          printf("wifi receive timeout\r\n");
        client.stop();
        is_connected_ = false;
        return RET_ERROR;
      }
    }
    size_t i = 0;
    // 受信したデータをバッファにコピー
    while (client.available()) {
      data[i++] = client.read();
    }
    *len = i;
    // 通信終了
    if (is_stop) client.stop();
    is_connected_ = true;
    return RET_OK;
  }

  RET transmitAndReceive(uint8_t *transmit_data, size_t transmit_data_len,
                         uint8_t *receive_data, size_t *receive_data_len) {
    // TODO: 送受信にリトライ機能つける
    WiFiClient client;
    // 通信確立
    if (!client.connect(host_, port_, TIMEOUT_MS)) {
      // 何回も失敗のログが出るとうざいので、1回だけ出るようにする
      if (is_connected_)
        printf("wifi connect error\r\n");
      is_connected_ = false;
      return RET_ERROR;
    }
    // 送信
    client.write(transmit_data, transmit_data_len);
    // 受信
    timer_->reset();
    // データを受信するまで待機。受信に一定時間以上かかった場合はタイムアウト
    while (client.available() == 0) {
      if (timer_->getElapsedTime() >= TIMEOUT_MS) {
        // 何回も失敗のログが出るとうざいので、1回だけ出るようにする
        if (is_connected_)
          printf("wifi receive timeout\r\n");
        client.stop();
        is_connected_ = false;
        return RET_ERROR;
      }
    }
    size_t i = 0;
    // 受信したデータをバッファにコピー
    while (client.available()) {
      receive_data[i++] = client.read();
    }
    *receive_data_len = i;
    // 通信終了
    client.stop();
    is_connected_ = true;
    return RET_OK;
  }

  const char *getSSID() { return ssid_; }
  const char *getPassword() { return password_; }
  const char *getHost() { return host_; }
  uint16_t getPort() { return port_; }
  IPAddress getLocalIP() { return local_ip_; }
  IPAddress getGateway() { return gateway_; }
  IPAddress getSubnet() { return subnet_; }
  bool getIsConnected() { return is_connected_; }
  timer::time_t getTimeoutMS() { return TIMEOUT_MS; }
};

}  // namespace driver
