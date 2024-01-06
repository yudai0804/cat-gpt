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
| bin2 |  5 | channel4 | 
| mode | 22 | 正論理、mode=1のときだけモーターが回る |

