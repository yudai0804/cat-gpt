# UML

## 全体
全体のステートマシン図  
![state_machine_all](./uml/out/state_machine/all/all.png)  
全体のシーケンス図(ACKは省略している)  
![sequence_all](./uml/out/sequence/all/all.png)  
## ステートマシン図
ステートに関してはメインステートとサブステートの2種類を用意する。  
メインステートはイベント駆動のものを、サブステートはイベント駆動というよりかは実装寄り(アクティビティ図より)なものにする。  
サブステートの粒度を実装寄り(アクティビティ図寄り)するのは、組み込みシステムの場合は粒度が実装寄り(アクティビティ図寄り)の方が分かりやすく、デバッグもしやすいため。  

メインステートの遷移はサーバー側が各基板のステートを監視しながら、サーバーから各基板のメインステートを操作する。  
サブステートは各基板内の判断で自動で遷移される。  
メインステートがManualのときは人間がスマホからすべてを操作するため、自動での遷移は行われない。  
すべてサーバーから遷移させる必要がある。  

## Rat
Idle  
![Idle](./uml/out/state_machine/rat/sub_state_idle/sub_state_idle.png)  
Search  
![Search](./uml/out/state_machine/rat/sub_state_search/sub_state_search.png)  
CaughtByCat  
![CaughtByCat](./uml/out/state_machine/rat/sub_state_caught_by_cat/sub_state_caught_by_cat.png)  
Feed  
![Feed](./uml/out/state_machine/rat/sub_state_feed/sub_state_feed.png)  
Manual  
![Manual](./uml/out/state_machine/rat/sub_state_manual/sub_state_manual.png)
