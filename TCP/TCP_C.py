import socket
import time
import sys

IP = '127.0.0.1'
PORT = 5050
CLI_ADDR = IP,PORT
BUFFSIZE = 1024

def client_socket():
    try:
        sk = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sk.connect(CLI_ADDR)
    except socket.error as msg:
        print(msg)
        sys.exit(1)

    print(sk.recv(BUFFSIZE))
    while True:
        massage = input("Client : ").strip()
        sk.send(massage.encode())

        print("client waiting...")
        server_reply = sk.recv(BUFFSIZE)
        print (server_reply.decode())
        if massage == "exit":
            break      

    sk.close()

if __name__ == "__main__":
    client_socket()

#socket client
