const keypress = require('keypress');
const { tcp, state_list, device_name } = require('./tcp_server')

// make `process.stdin` begin emitting "keypress" events
keypress(process.stdin);

// listen for the "keypress" event
process.stdin.on('keypress', function(ch, key) {

  // 4. key. で入力情報を取得
  // console.log('got "keypress"', key);
  // console.log("ch", ch);

  if (ch == 'c') tcp.transmitChangeState(device_name.getRat(), state_list.getStateByName("Manual", "Manual"));

  //Ctrl + c入力のときの処理
  if (key && key.ctrl && key.name == 'c') {
    process.stdin.exit();
    //強制終了するなら　process.stdin.exit();
  }
});

process.stdin.setRawMode(true);
process.stdin.resume();

