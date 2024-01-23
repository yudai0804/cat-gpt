const { Socket } = require('dgram');
const net = require('net');

class TCPServer {
  #port;
  #ip_address;
  constructor(port, ip_address) {
    this.#port = port;
    this.#ip_address = ip_address;
    const server = net.createServer(socket => {
      socket.on('data', data => {
        console.log(`${data} from ${socket.remoteAddress}`);
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
