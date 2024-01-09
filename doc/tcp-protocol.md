
# tcp-protocol

マイコン-Server間のデータ通信のプロトコル  
**通信の開始は必ずマイコン側から行われる。**  
何かに準拠したものではなく、適当に考えたなんちゃってプロトコルなので注意  

通信方式はTCP、データ型はuint8_tを想定  
このプロトコルはサーバー側(nodejs)でも使われるものであり、JavaScriptはC言語と比較してbit操作が苦手(らしい)ため、今回はbit操作を不要とするプロトコルを作成する。  
ACKは返すものとする。  
通信の簡略化のため、データをまとめて送るということはしないようにする。  

TODO: 使用するIP及びポート番号が決まったらまとめる。  

各基板にはIdle状態とNoConnectという状態があり、Serverとの通信が成立した際はIdle状態となり、Serverとの通信に失敗した場合はNoConnectという状態になる。  
状態遷移中はChangeState状態となる。  

| offset | type | name | details |
| - | - | - | - |
| 0 | uint8_t | order_number(命令数) | |
| 1 | uint8_t | header1 | |
| 2 | uint8_t | length1 | lengthは0も設定可能 |
| 3 ~ 3 + (length1 - 1) | | data1 | |
| | | order_number分のheader,length,dataが来る | |

## header

headerのMSBはACK用とする。MSBが1である場合はACKである。  
そのため、0x00~0x80までが自由に使える領域となる。  

| number | name | details |
| - | - | - |
| 0x00 | StateInformation | 一定時間おきに各基板から送信される。 |
| 0x01 | ChangeState | ステートを移動する |
| 0x02 | RequestChangeState | ステートを移動していいかServerに尋ねる |
| 0x03 | SetSearchMode | Ratの探索モードを設定。|
| 0x04 | SetAppealMode | Ratがアピールするときのモードを設定 |
| 0x05 | SetFoodQuantity | Feederが出す餌の量を設定 |
| 0x06 | ManualMove | manualステートのときのみ有効。  設定したしたパラメーターでRatが移動する。 |
| 0x07 | ManualFeed | manualステートのときのみ有効。  設定した量の餌をFeederが出す |

## StateInformation  

基板->Server  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x00(StateInformation) |
| 1 | uint8_t | length | 2 |
| 2 | uint8_t | main_state | |
| 3 | uint8_t | sub_state | |

### StateInformation ACK

Server->基板  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x80 + 0x00(StateInformation) |
| 1 | uint8_t | length | 0 |

## ChangeState  

Server->基板  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x01(ChangeState) |
| 1 | uint8_t | length | 2 |
| 2 | uint8_t | main_state | |
| 3 | uint8_t | sub_state | |

### ChangeState ACK

基板->Server  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x80 + 0x01(ChangeState) |
| 1 | uint8_t | length | 3 |
| 2 | uint8_t | is_success | 成功:1,失敗:0 |
| 3 | uint8_t | main_state | 現在の値 |
| 4 | uint8_t | sub_state | 現在の値 |

## RequestChangeState  

基板->Server  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x02(RequestChangeState) |
| 1 | uint8_t | length | 2 |
| 2 | uint8_t | main_state | |
| 3 | uint8_t | sub_state | |

### RequestChangeState ACK

Server->基板  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x80 + 0x02(RequestChangeState) |
| 1 | uint8_t | length | 3 |
| 2 | uint8_t | is_ok | ok:1,だめ:0 |
| 3 | uint8_t | main_state | 遷移するステート |
| 4 | uint8_t | sub_state | 遷移するステート |

## SetSearchMode  

Server->基板  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x03(SetSearchMode) |
| 1 | uint8_t | length | 1 |
| 2 | uint8_t | mode | |

### SetSearchMode ACK

基板->Server  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x80 + 0x03(SetSearchMode) |
| 1 | uint8_t | length | 2 |
| 2 | uint8_t | is_success | 成功:1,失敗:0 |
| 3 | uint8_t | mode | 現在の値 |

## SetAppealMode  

Server->基板  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x04(SetAppealMode) |
| 1 | uint8_t | length | 1 |
| 2 | uint8_t | mode | |

### SetAppealMode ACK

基板->Server  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x80 + 0x04(SetAppealMode) |
| 1 | uint8_t | length | 2 |
| 2 | uint8_t | is_success | 成功:1,失敗:0 |
| 3 | uint8_t | mode | 現在の値 |

## SetFoodQuantity  

Server->基板  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x05(SetFoodQuantity) |
| 1 | uint8_t | length | 1 |
| 2 | uint8_t | quantity |単位は[g] |

### SetFoodQuantity ACK

基板->Server  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x80 + 0x05(SetFoodQuantity) |
| 1 | uint8_t | length | 2 |
| 2 | uint8_t | is_success | 成功:1,失敗:0 |
| 3 | uint8_t | quantity | 現在の値 |

## ManualMove  

Server->基板  
メインステートがmanualステートのときのみ有効となる  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x06(ManualMove) |
| 1 | uint8_t | length | 8 |
| 2 | float | velocity(31~24bit) | |
| 3 | | velocity(23~16bit) | |
| 4 | | velocity(15~8bit) | |
| 5 | | velocity(7~0bit) | |
| 6 | float | omega(31~24bit) | |
| 7 | | omega(23~16bit) | |
| 8 | | omega(15~8bit) | |
| 9 | | omega(7~0bit) | |

### ManualMode ACK

基板->Server  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x06(ManualMove) |
| 1 | uint8_t | length | 9 |
| 2 | uint8_t | is_success | 成功:1,失敗:0 |
| 3 | float | velocity(31~24bit) | 現在の値 |
| 4 | | velocity(23~16bit) | |
| 5 | | velocity(15~8bit) | |
| 6 | | velocity(7~0bit) | |
| 7 | float | omega(31~24bit) | 現在の値 |
| 8 | | omega(23~16bit) | |
| 9 | | omega(15~8bit) | |
| 10 | | omega(7~0bit) | |

## ManualFeed  

Server->基板  
メインステートがmanualステートのときのみ有効となる  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x07(ManualFeed) |
| 1 | uint8_t | length | 1 |
| 2 | uint8_t | quantity |単位は[g] |

### ManualFeed ACK

基板->Server  
| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | header | 0x80 + 0x07(ManualFeed) |
| 1 | uint8_t | length | 2 |
| 2 | uint8_t | is_success | 成功:1,失敗:0 |
| 3 | uint8_t | quantity | 現在の値 |
