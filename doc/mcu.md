
# 概要

マイコン:ESP32  
開発環境:PlatformIO

# 使用しているライブラリ  

[vl53l0x-arduino](https://github.com/pololu/vl53l0x-arduino)  

# ピンアサイン  

| role | pin | comment(option) |
| - | - | - |
| led_red | 12 | |
| led_white | 14 | |
| limit_switch | 15 | pullup,bootと共有 |
| ui_switch | 27 | pullup |
| scl | 17 | 100kbps |
| sda | 16 | 100kbps |
| rgbled | 4 | たぶんDMAを使うはず |
| buzzer | 23 | channel0 |
| ain1 | 21 | channel1 |
| ain2 | 19 | channel2 |
| bin1 | 18 | channel3 |
| bin2 | 5 | channel4 |
| mode | 22 | 正論理、mode=1のときだけモーターが回る |

# Wi-Fiの情報について

外部にもらしては行けない情報に関しては、各プロジェクトのsrcディレクトリにあるprivate_information.hというファイルにマクロとして定義している。  
private_information.hはgitignoreされているため、Gitでは追跡されていない。  

イメージ

```
/**
 * @file private_information.h
 * @brief 外部に公開したらまずい情報が入っている。
 */
#include "Arduino.h"
#include "IPAddress.h"

#pragma once

#define SSID "SSID!!!!!"
#define PASSWORD "PASSWORD!!!!!!"
#define HOST "192.168.10.111"
#define PORT 5000
#define LOCAL_IP IPAddress(192, 168, 10, 123)
#define GATEWAY IPAddress(192, 168, 10, 1)
#define SUBNET IPAddress(255, 255, 255, 0)

```
