const net = require('net');
const Buffer = require('buffer/').Buffer;

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

class TCPServer {
  #port;
  #ip_address;
  #RAT_IP;
  #FEEDER_IP;
  /** @type {CommandList} */
  #command_list;
  /** @type {StateList} */
  #state_list;
  #rat_transmit_queue;
  #feeder_transmit_queue;
  /** @type {State} */
  #rat_state;
  /** @type {State} */
  #feeder_state;

  #addOrder(ip, header, data_array = []) {
    if (ip == this.#RAT_IP) {
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
    let cmd = this.#command_list.getCommandByValue(header);
    switch (cmd.value) {
      case this.#command_list.getCommandByValue("StateInformation").value:
        // TODO: おかしなstateが来た時は値を更新しない処理を追加する
        let tmp_state = this.#state_list.getStateByValue(data_array[0], data_array[1]);
        if (ip == this.#RAT_IP) this.#rat_state = tmp_state;
        else if (ip == this.#FEEDER_IP) this.#feeder_state = tmp_state;
        // ACKを返す
        let ack = this.#command_list.getCommandByValue("StateInformationACK").value;
        this.#addOrder(ip, ack, []);
        break;
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

  #onReceive(ip, data) {
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

  constructor(port, ip_address) {
    this.#port = port;
    this.#ip_address = ip_address;
    this.#RAT_IP = "192.168.10.111";
    this.#command_list = CommandList();
    this.#state_list = StateList();
    const server = net.createServer(socket => {
      socket.on('data', data => {
        console.log(`${data} from ${socket.remoteAddress}`);
        this.#onReceive(socket.remoteAddress, data);
        let tx_data = this.#transmit(socket.remoteAddress);
        socket.write(tx_data);
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
