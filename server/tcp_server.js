const net = require('net');
const { clearTimeout, clearInterval } = require('timers');
const Buffer = require('buffer/').Buffer;
const keypress = require('keypress');

class Device {
  constructor() { }
  getRat() { return "Rat"; }
  getFeeder() { return "Feeder"; }
}
const device_name = new Device();

class Command {
  name;
  value;
  constructor(name, value) {
    this.name = name;
    this.value = value;
  }
}
class CommandList {
  #command;
  constructor() {
    this.#command = [
      new Command("StateInformation", 0x00),
      new Command("ChangeState", 0x01),
      new Command("RequestChangeState", 0x02),
      new Command("SetSearchMode", 0x03),
      new Command("SetAppealMode", 0x04),
      new Command("SetFoodQuantity", 0x05),
      new Command("ManualMove", 0x06),
      new Command("ManualFeed", 0x07),
      new Command("StartBuzzer", 0x08)
    ];
    // ACKを追加
    // ACKのvalueは0x80
    const length = this.#command.length;
    for (let i = 0; i < length; i++) {
      const cmd = this.#command[i];
      this.#command.push(new Command(cmd.name + "ACK", cmd.value + 0x80));
    }
  }

  getCommand() { return this.#command; }

  getCommandByName(name) {
    return this.#command.find(cmd => { return cmd.name == name });
  }

  getCommandByValue(value) {
    return this.#command.find(cmd => { return cmd.value == value });
  }
}

const command_list = new CommandList();

class State {
  main_state;
  main_value;
  sub_state;
  sub_value;
  constructor(main_state, main_value, sub_state, sub_value) {
    this.main_state = main_state;
    this.main_value = main_value;
    this.sub_state = sub_state;
    this.sub_value = sub_value;
  }
}

class StateList {
  #state;
  constructor() {
    this.#state = [
      //        main_state, main_value, sub_state, sub_value
      new State("Idle", 0x00, "Idle", 0x00),
      new State("Idle", 0x00, "NoConnect", 0x01),
      new State("Idle", 0x00, "ChangingState", 0x02),
      new State("Search", 0x01, "Start", 0x00),
      new State("Search", 0x01, "Search", 0x01),
      new State("Search", 0x01, "DetectObstacle", 0x02),
      new State("Search", 0x01, "AvoidObstacle", 0x03),
      new State("Search", 0x01, "AppealToCat", 0x04),
      new State("Search", 0x01, "Finish", 0x05),
      new State("CaughtByCat", 0x02, "Start", 0x00),
      new State("CaughtByCat", 0x02, "Caught", 0x01),
      new State("CaughtByCat", 0x02, "Finish", 0x02),
      new State("Feed", 0x03, "Start", 0x00),
      new State("Feed", 0x03, "AppealToCat", 0x01),
      new State("Feed", 0x03, "Feed", 0x02),
      new State("Feed", 0x03, "Finish", 0x03),
      new State("Manual", 0x04, "Manual", 0x00),
      new State("Error", 0x05, "Error", 0x00)
    ];
  }

  getState() { return this.#state; }

  getStateByName(main, sub = undefined) {
    const main_filtered = this.#state.filter(state => { return state.main_state == main });
    // subが入力されていなかった場合はmain_stateをフィルターしたものを返す
    if (sub == undefined) return main_filtered;
    // subが入力されていた場合はmainとsubが一致したものを返す
    const match_main_and_sub = main_filtered.find(state => { return state.sub_state == sub });
    return match_main_and_sub;
  }

  getStateByValue(main, sub = undefined) {
    const main_filtered = this.#state.filter(state => { return state.main_value == main });
    // subが入力されていなかった場合はmain_stateをフィルターしたものを返す
    if (sub == undefined) return main_filtered;
    // subが入力されていた場合はmainとsubが一致したものを返す
    const match_main_and_sub = main_filtered.find(state => { return state.sub_value == sub });
    return match_main_and_sub;
  }
}

const state_list = new StateList();

class CheckACK {
  #is_wait_ack;
  #timer_id;
  #retry_count;
  #MAX_RETRY_COUNT;
  constructor() {
    this.#is_wait_ack = [];
    this.#timer_id = [];
    this.#retry_count = [];
    this.#MAX_RETRY_COUNT = 1;
    this.clearACK();
  }

  startACK(header, func) {
    this.#is_wait_ack[header] = 1;
    this.#retry_count[header] = 0;
    func();
    this.#timer_id[header] = setInterval(() => {
      if (this.#retry_count[header] > this.#MAX_RETRY_COUNT) {
        console.log("ack error");
        clearInterval(this.#timer_id[header]);
        this.#is_wait_ack[header] = 2;
      } else {
        func();
        this.#retry_count[header]++;
      }
    }, 1000);
  }

  receiveACK(header) {
    if (this.#is_wait_ack[header] == 1) {
      clearInterval(this.#timer_id[header]);
      this.#retry_count[header] = 0;
      this.#is_wait_ack[header] = 0;
    }
  }

  clearACK() {
    let command = command_list.getCommand();
    for (let i = 0; i < command.length; i++) {
      this.#is_wait_ack[command[i].value] = 0;
      this.#retry_count[command[i].value] = 0;
      clearInterval(this.#timer_id[command[i]]);
    }
  }

  getIsWaitAck(header) {
    return this.#is_wait_ack[header];
  }
}

class TCPServer {
  #port;
  #ip_address;
  #RAT_IP;
  #FEEDER_IP;
  #rat_transmit_queue;
  #feeder_transmit_queue;
  /** @type {State} */
  #rat_state;
  /** @type {State} */
  #feeder_state;
  #is_rat_alive;
  #is_feeder_alive;
  #rat_timer_id;
  #feeder_timer_id;
  #TIMEOUT;
  /** @CheckACK */
  #rat_check_ack;
  /** @CheckACK */
  #feeder_check_ack;

  #addOrder(ip, header, data_array = []) {
    if (ip == this.#RAT_IP) {
      if (this.#rat_transmit_queue.length == 0) {
        this.#rat_transmit_queue[0] = 1;
      } else {
        this.#rat_transmit_queue[0]++;
      }
      this.#rat_transmit_queue.push(header);
      this.#rat_transmit_queue.push(data_array.length);
      for (let i = 0; i < data_array.length; i++) {
        this.#rat_transmit_queue.push(data_array[i]);
      }
    } else if (ip == this.#FEEDER_IP) {
      if (this.#feeder_transmit_queue.length == 0) {
        this.#feeder_transmit_queue[0] = 1;
      } else {
        this.#feeder_transmit_queue[0]++;
      }
      this.#feeder_transmit_queue.push(header);
      this.#feeder_transmit_queue.push(data_array.length);
      for (let i = 0; i < data_array.length; i++) {
        this.#feeder_transmit_queue.push(data_array[i]);
      }
    } else {
      console.log("ip error");
    }
  }

  #receive(ip, header, data_array) {
    // ack check
    if (ip == this.#RAT_IP) {
      this.#rat_check_ack.receiveACK(header);
    } else if (ip == this.#FEEDER_IP) {
      this.#feeder_check_ack.receiveACK(header);
    }
    let cmd = command_list.getCommandByValue(header);
    switch (cmd.value) {
      case command_list.getCommandByName("StateInformation").value: {
        // TODO: おかしなstateが来た時は値を更新しない処理を追加する
        let tmp_state = state_list.getStateByValue(data_array[0], data_array[1]);
        if (ip == this.#RAT_IP) this.#rat_state = tmp_state;
        else if (ip == this.#FEEDER_IP) this.#feeder_state = tmp_state;
        // ACKを返す
        let ack = command_list.getCommandByName("StateInformationACK").value;
        // console.log("print state information")
        // console.log(this.#rat_state);
        this.#addOrder(ip, ack, []);
        break;
      }
      case command_list.getCommandByName("ChangeStateACK").value: {
        let tmp_state = state_list.getStateByValue(data_array[1], data_array[2]);
        if (ip == this.#RAT_IP) this.#rat_state = tmp_state;
        else if (ip == this.#FEEDER_IP) this.#feeder_state = tmp_state;
        // console.log("print change state ack");
        // console.log(this.#rat_state);
        break;
      }
      case command_list.getCommandByName("RequestChangeState").value: {
        let tmp_state = state_list.getStateByValue(data_array[0], data_array[1]);
        if (ip == this.#RAT_IP) this.#rat_state = tmp_state;
        else if (ip == this.#FEEDER_IP) this.#feeder_state = tmp_state;
        // TODO: ここで処理を実行すること
        // 今は仮でリクエストされたステートをそのまま許可している
        console.log("request change state");
        console.log(tmp_state)
        let ack = command_list.getCommandByName("RequestChangeStateACK").value;
        this.#addOrder(ip, ack, [1, data_array[0], data_array[1]]);
        break;
      }
    }
  }

  #decodeOrder(ip, data) {
    const order_number = data[0];
    let header = data[1];
    let length = data[2];
    let index = 3;
    for (let i = 0; i < order_number; i++) {
      // 命令に該当するするデータをコピー
      let data_array = []
      for (let j = 0; j < length; j++) {
        data_array.push(data[index++]);
      }
      this.#receive(ip, header, data_array);
      // headerとlengthを更新
      header = data[index++];
      length = data[index++];
    }
  }

  #clearTimer(ip, data) {
    if (ip == this.#RAT_IP) {
      if (!this.#is_rat_alive) {
        this.#is_rat_alive = 1;
        console.log("rat connect");
      }
      clearTimeout(this.#rat_timer_id);
      this.#rat_timer_id = setTimeout(() => {
        this.#is_rat_alive = 0;
        console.log("rat connect lost");
      }, this.#TIMEOUT);
    } else if (ip == this.#FEEDER_IP) {
      if (!this.#is_feeder_alive) {
        this.#is_feeder_alive = 1;
        console.log("feeder connect");
      }
      clearTimeout(this.#feeder_timer_id);
      this.#feeder_timer_id = setTimeout(() => {
        this.#is_feeder_alive = 0;
        console.log("feeder connect lost");
      }, this.#TIMEOUT);

    }
  }

  #onReceive(ip, data) {
    this.#clearTimer(ip, data);
    this.#decodeOrder(ip, data);
  }

  #transmit(ip) {
    if (ip == this.#RAT_IP) {
      let ret = Buffer.from(this.#rat_transmit_queue);
      // bufferをクリア
      this.#rat_transmit_queue = [];
      return ret;
    } else if (ip == this.#FEEDER_IP) {
      let ret = Buffer.from(this.#feeder_transmit_queue);
      // bufferをクリア
      this.#feeder_transmit_queue = [];
      return ret;
    }
  }

  /** 
   * @param {State} state
   */
  transmitChangeState(name, state) {
    let transmit = () => {
      let data = [state.main_value, state.sub_value];
      let ip;
      if (name == device_name.getRat()) {
        ip = this.#RAT_IP;
      }
      else if (name == device_name.getFeeder()) { ip = this.#FEEDER_IP; }
      let header = command_list.getCommandByName("ChangeState");
      this.#addOrder(ip, header.value, data);
    };
    if (name == device_name.getRat()) {
      this.#rat_check_ack.startACK(command_list.getCommandByName("ChangeStateACK").value, transmit);
    } else if (name == device_name.getFeeder()) {
      this.#feeder_check_ack.startACK(command_list.getCommandByName("ChangeStateACK").value, transmit);
    }
  }

  transmitManualMove(velocity, angular_velocity) {
    // 命令が連続して来たとき用の対策
    if (this.#rat_check_ack.getIsWaitAck(command_list.getCommandByName("ManualMoveACK").value) == 1) {
      // console.log("loop", this.#rat_check_ack.getIsWaitAck(command_list.getCommandByName("ManualMoveACK").value));
      return;
    }

    let transmit = () => {
      let float32_data = new Float32Array([velocity, angular_velocity]);
      let data = new Buffer.from(float32_data.buffer);
      let ip = this.#RAT_IP;
      let header = command_list.getCommandByName("ManualMove");
      this.#addOrder(ip, header.value, data);
    };
    this.#rat_check_ack.startACK(command_list.getCommandByName("ManualMoveACK").value, transmit);
  }

  transmitManualFeed(quantity) {
    // 命令が連続して来たとき用の対策
    if (this.#feeder_check_ack.getIsWaitAck(command_list.getCommandByName("ManualFeedACK").value) == 1) {
      return;
    }

    let transmit = () => {
      let float32_data = new Float32Array([quantity]);
      let data = new Buffer.from(float32_data.buffer);
      let ip = this.#RAT_IP;
      let header = command_list.getCommandByName("ManualFeed");
      this.#addOrder(ip, header.value, data);
    };
    this.#feeder_check_ack.startACK(command_list.getCommandByName("ManualFeedACK").value, transmit);

  }

  constructor(port, my_ip, rat_ip, feeder_ip) {
    this.#port = port;
    this.#ip_address = my_ip;
    this.#RAT_IP = rat_ip;
    console.log("rat_ip", this.#RAT_IP);
    this.#FEEDER_IP = feeder_ip;
    console.log("feeder_ip", this.#FEEDER_IP);
    this.#TIMEOUT = 1000;
    this.#rat_check_ack = new CheckACK();
    // IPが異常な値でないかチェック
    if (this.#RAT_IP == this.#FEEDER_IP)
      console.log("ip address error");
    this.#rat_transmit_queue = [];
    this.#feeder_transmit_queue = [];

    this.#rat_timer_id = setTimeout(() => {
      this.#is_rat_alive = 0;
      console.log("rat connect lost");
    }, this.#TIMEOUT);

    this.#feeder_timer_id = setTimeout(() => {
      this.#is_feeder_alive = 0;
      console.log("feeder connect lost");
    }, this.#TIMEOUT);
    let id;
    const server = net.createServer(socket => {
      socket.on('data', data => {
        //console.log(start_date - new Date())
        // console.log(`${data} from ${socket.remoteAddress}`);
        //console.log(data);
        this.#onReceive(socket.remoteAddress, data);
        let tx_data = this.#transmit(socket.remoteAddress);
        if (tx_data.length != 0)
          socket.write(tx_data);
        else
          console.log("tx_data length is 0")
        clearTimeout(id);
        id = setTimeout(() => { this.#rat_check_ack.clearACK(); console.log("clear ack") }, 5000);
      });

      socket.on('close', () => {
        // console.log("client close connection");
      })
    });
    server.listen(this.#port, this.#ip_address);
    console.log(`create server: port = ${this.#port}, ip = ${this.#ip_address}`);
  }

  getIsAlive(name) {
    if (name == device_name.getRat()) return this.#is_rat_alive;
    else if (name == device_name.getFeeder()) return this.#is_feeder_alive;
    return undefined;
  }

  getState(name) {
    if (name == device_name.getRat()) return this.#rat_state;
    else if (name == device_name.getFeeder()) return this.#feeder_state;
    return undefined;
  }

}

// const tcp = new TCPServer(5000, '192.168.227.10', '192.168.227.123', "192.168.100.123");
const tcp = new TCPServer(5000, '192.168.10.111', '192.168.10.123', "192.168.100.124");

module.exports = { tcp, state_list, device_name };

/*
setInterval(() => {
  if (tcp.getIsAlive(device_name.getRat()))
    tcp.transmitChangeState(device_name.getRat(), state_list.getStateByName("Idle", "Idle"))
}, 5000);
*/
/*
setInterval(() => {
  if (tcp.getIsAlive(device_name.getRat())) {
    let v = Math.random();
    let av = Math.random();
    console.log(`v = ${v}, av = ${av}`);
    tcp.transmitManualMove(v, av);
  }
}, 5000);
*/
