
# tcp-protocol

マイコン-Server間のデータ通信のプロトコル  
何かに準拠したものではなく、適当に考えたなんちゃってプロトコルなので注意  

通信方式はTCP、データ型はuint8_tを想定  
このプロトコルはサーバー側(nodejs)でも使われるものであり、JavaScriptはC言語と比較してbit操作が苦手(らしい)ため、今回はbit操作を不要とするプロトコルを作成する。  
ACKは返すものとする。  
通信の簡略化のため、データをまとめて送るということはしないようにする。  

TODO: 使用するIP及びポート番号が決まったらまとめる。  

各基板にはIdle状態とWaitForConnectionという状態があり、Serverとの通信が成立した際はIdle状態となり、Serverとの通信に失敗した場合はWaitForConnectionという状態になる。  

| offset | type | name |
| - | - | - |
| 0 | uint8_t | header |
| 1~n | | データ |

## header

headerのMSBはACK用とする。MSBが1である場合はACKである。  
そのため、0x00~0x80までが自由に使える領域となる。  

| number | name | details |
| - | - | - |
| 0x00 | StateInformation | 一定時間おきに各基板から送信される。|
| 0x01 | ChangeState | ステートを移動する |
| 0x02 | SetSearchMode | ネズミの探索モードを設定。|
| 0x03 | SetAppelMode | ネズミがアピールするときのモードを設定 |
| 0x04 | SetFoodQuantity | Feederが出す餌の量を設定 |

## StateInformation  

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x00(StateInformation) |
| 1 | uint8_t | main_state | |
| 2 | uint8_t | sub_state | |

### StateInformation ACK

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x80 + 0x00(StateInformation) |
| 1 | uint8_t | dummy_data | 1 |

## ChangeState  

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x01(ChangeState) |
| 1 | uint8_t | main_state | |
| 2 | uint8_t | sub_state | |

### ChangeState ACK

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x80 + 0x01(ChangeState) |
| 1 | uint8_t | is_success | 成功:1,失敗:0 |

## SetSearchMode  

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x02(SetSearchMode) |
| 1 | uint8_t | mode | |

### SetSearchMode ACK

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x80 + 0x02(SetSearchMode) |
| 1 | uint8_t | is_success | 成功:1,失敗:0 |

## SetAppelMode  

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x03(SetAppelMode) |
| 1 | uint8_t | mode | |

### SetAppelMode ACK

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x80 + 0x03(SetAppelMode) |
| 1 | uint8_t | is_success | 成功:1,失敗:0 |

## SetFoodQuantity  

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x03(SetFoodQuantity) |
| 1 | uint8_t | quantity | 単位はグラム |

### SetFoodQuantity ACK

| offset | type | role | details |
| - | - | - | - |
| 0 | uint8_t | heaader | 0x80 + 0x03(SetFoodQuantity) |
| 1 | uint8_t | is_success | 成功:1,失敗:0 |
