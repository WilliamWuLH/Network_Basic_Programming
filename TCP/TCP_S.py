# -*- coding:utf-8 -*-

import socket
ip_port = ('127.0.0.1',5050)

sk = socket.socket()
sk.bind(ip_port)
sk.listen(5)

while True:
    print('server waiting...')
    conn,addr = sk.accept()

    client_data = conn.recv(1024)
    print(client_data.decode())
    conn.sendall("Server : OKK".encode())

    conn.close()

#socket server
