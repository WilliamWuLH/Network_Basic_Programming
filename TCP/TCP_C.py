# -*- coding:utf-8 -*-

import socket
ip_port = ('127.0.0.1',5050)

while True:
    sk = socket.socket()
    sk.connect(ip_port)
    massage = input("Client : ").strip()
    if massage == "exit":
        break
    sk.sendall(("Client : " + massage).encode())
    print("client waiting...")
    server_reply = sk.recv(1024)
    print (server_reply.decode())
    sk.close()

#socket client
