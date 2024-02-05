const net = require('net');
const { clearTimeout } = require('timers');
const Buffer = require('buffer/').Buffer;

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
      new Command("ChangeState", 0x01)
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
      new State("Search", 0x01, "Start", 0x00)
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
  // TODO: alive実装する
  #is_rat_alive;
  #is_feeder_alive;
  #rat_timer_id;
  #feeder_timer_id;
  #TIMEOUT;

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
      // TODO: feederの実装する
    } else {
      console.log("ip error");
    }
  }

  #receive(ip, header, data_array) {
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
        console.log("print change state ack");
        console.log(this.#rat_state);
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
      // TODO: feederも実装する
    }
  }

  /** 
   * @param {State} state
   */
  transmitChangeState(name, state) {
    let data = [state.main_value, state.sub_value];
    let ip;
    if (name == device_name.getRat()) { ip = this.#RAT_IP; }
    else if (name == device_name.getFeeder()) { ip = this.#FEEDER_IP; }
    let header = command_list.getCommandByName("ChangeState");
    this.#addOrder(ip, header.value, data);
  }

  constructor(port, my_ip, rat_ip, feeder_ip) {
    this.#port = port;
    this.#ip_address = my_ip;
    this.#RAT_IP = rat_ip;
    this.#FEEDER_IP = feeder_ip;
    this.#TIMEOUT = 1000;
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

    const server = net.createServer(socket => {
      socket.on('data', data => {
        // console.log(`${data} from ${socket.remoteAddress}`);
        // console.log(data);
        this.#onReceive(socket.remoteAddress, data);
        let tx_data = this.#transmit(socket.remoteAddress);
        if (tx_data.length != 0)
          socket.write(tx_data);
        else
          console.log("tx_data length is 0")
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
}

// tcp = new TCPServer(5000, '192.168.227.10', '127.168.227.123', "192.168.100.123");
const tcp = new TCPServer(5000, '192.168.10.111', '192.168.10.123', "192.168.100.123");

/*
setInterval(() => {
  if (tcp.getIsAlive(device_name.getRat()))
    tcp.transmitChangeState(device_name.getRat(), state_list.getStateByName("Idle", "NoConnect"))
}, 5000);
*/
