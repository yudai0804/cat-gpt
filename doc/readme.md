# UML
## ステートマシン図
ステートに関してはメインステートとサブステートの2種類を用意する。  
メインステートはイベント駆動のものを、サブステートはイベント駆動というよりかは実装寄り(粒度細かめ)なものにする。  
サブステートの粒度を実装寄り(細かく)するのは、組み込みシステムの場合は粒度が実装寄り(細かい)ほうが分かりやすく、デバッグもしやすいため。  

全体のステートマシン図  
![state_machine_all](./uml/out/state_machine/all/all.png)  

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
![Manual](./uml/out/state_machine/rat/sub_state_manual/)
