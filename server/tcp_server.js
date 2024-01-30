const net = require('net');

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
    console.log(this.#command[0])
    // ACKを追加
    // ACKのvalueは0x80
    const length = this.#command.length;
    for (let i = 0; i < length; i++) {
      const cmd = this.#command[i];
      this.#command.push(new CommandType(cmd.name + "ACK", cmd.value + 0x80));
    }
  }

  getCommandByName(name) {
    return this.#command.find(cmd => { return cmd.name == name });
  }

  getCommandByValue(value) {
    return this.#command.find(cmd => { return cmd.value == value });
  }
}

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
      //           main_state, main_value, sub_state, sub_value
      new State("Idle", 0x00, "Idle", 0x00),
      new State("Idle", 0x00, "NoConnect", 0x01),
      new State("Idle", 0x00, "ChangingState", 0x02),
    ];
  }
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

class TCPServer {
  #port;
  #ip_address;
  #RAT_IP;
  #transmit_queue;
  /** @type {StateType} */
  #rat_state;
  /** @type {StateType} */
  #feeder_state;
  #receive(ip, header, data_array) {
    switch (header) {

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

  #transmitAndReceive(ip, data) {
    this.#decodeOrder(ip, data);
  }

  constructor(port, ip_address) {
    this.#port = port;
    this.#ip_address = ip_address;
    this.#RAT_IP = "192.168.10.111";
    const server = net.createServer(socket => {
      socket.on('data', data => {
        console.log(`${data} from ${socket.remoteAddress}`);
        this.#receive(data, socket.remoteAddress);
      });

      socket.on('close', () => {
        console.log("client close connection");
      })
    });
    server.listen(this.#port, this.#ip_address);
    console.log(`create server: port = ${this.#port}, ip = ${this.#ip_address}`);
  }
}

tcp = new TCPServer(5000, 'localhost');
