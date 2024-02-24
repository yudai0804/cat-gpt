const keypress = require('keypress');
const { tcp, state_list, device_name } = require('./tcp_server')

let v = 0;
let a = 0.1;

// make `process.stdin` begin emitting "keypress" events
keypress(process.stdin);

// listen for the "keypress" event
process.stdin.on('keypress', function(ch, key) {

  // 4. key. で入力情報を取得
  // console.log('got "keypress"', key);
  // console.log("ch", ch);

  if (ch == 'c') {
    tcp.transmitChangeState(device_name.getRat(), state_list.getStateByName("Manual", "Manual"));
    tcp.transmitChangeState(device_name.getFeeder(), state_list.getStateByName("Manual", "Manual"));
  } else if (ch == 'w') {
    v += a;
    tcp.transmitManualMove(v, 0);
    console.log(v);
  } else if (ch == 's') {
    v -= a;
    tcp.transmitManualMove(v, 0);
    console.log(v);
  } else if (ch == 'e') {
    v = 0;
    console.log(v);
  } else if (ch == 'f') {
    tcp.transmitManualFeed(5.0);
    console.log("manual feed");
  } else if (ch == 'S') {
    console.log("rat", tcp.getState(device_name.getRat()));
    console.log("feeder", tcp.getState(device_name.getFeeder()));
  }

  //Ctrl + c入力のときの処理
  if (key && key.ctrl && key.name == 'c') {
    process.stdin.exit();
    //強制終了するなら　process.stdin.exit();
  }
});

process.stdin.setRawMode(true);
process.stdin.resume();
let s = new Date();
setInterval(() => {
  if (tcp.getIsAlive(device_name.getRat())) {
    tcp.transmitManualMove(v, 0);
  }
}, 50)
