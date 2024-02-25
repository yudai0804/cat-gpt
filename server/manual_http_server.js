const { tcp, state_list, device_name } = require('./tcp_server')

const express = require('express');　// expressを準備
const app = express();
const cors = require('cors');
const bodyParser = require('body-parser');
const ip = "192.168.10.111";
const port = 3000;

// express に ejs のテンプレートエンジンを設定
app.set("view engine", "ejs");

app.get("/", function(req, res) {
  res.render("index"); // これで /views/index.ejs をレンダリング
});

app.use(cors())
app.use(bodyParser.json());

let is_client_connected = false;

function clientTimeout() {
  return setTimeout(() => {
    is_client_connected = false;
    console.log("client connect error");
  }, 2000);
}

let timer_id = clientTimeout();

tcp.transmitChangeState(device_name.getRat(), state_list.getStateByName("Manual", "Manual"))

app.post("/api/json", (req, res, next) => {
  is_client_connected = true;
  clearTimeout(timer_id);
  timer_id = clientTimeout();
  // console.log(req.body)
  // ダミーデータを送信
  res.json({});
  // マイコンに命令を送信
  if (tcp.getIsAlive(device_name.getRat())) {
    if (tcp.getState(device_name.getRat()) != state_list.getStateByName("Manual", "Manual")) {
      // tcp.transmitChangeState(device_name.getRat(), state_list.getStateByName("Manual", "Manual"))
    }
    tcp.transmitManualMove(req.body.velocity, req.body.angular_velocity);
  }
  if (tcp.getIsAlive(device_name.getFeeder())) {
    if (tcp.getState(device_name.getFeeder()) != state_list.getStateByName("Manual", "Manual")) {
      // tcp.transmitChangeState(device_name.getFeeder(), state_list.getStateByName("Manual", "Manual"))
    }
    if (req.body.feeder_flag == 1) {
      tcp.transmitManualFeed(req.body.feeder_flag)
    }
  }
})

app.listen(port, ip, () => {
  console.log(`manual http server listening at http://${ip}:${port}`);
})
