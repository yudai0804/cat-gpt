// const { tcp, state_list, device_name } = require('./tcp_server')

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

app.post("/api/json", (req, res, next) => {
  // console.log(req.body)
  // ダミーデータを返却
  res.json({});
})

app.listen(port, ip, () => {
  console.log(`Example app listening at http://${ip}:${port}`);
})
