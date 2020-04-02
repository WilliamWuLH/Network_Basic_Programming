import socket
import threading
import time
import sys


IP = '127.0.0.1'
PORT = 5050
SER_ADDR = IP,PORT
BUFFSIZE = 1024

def server_socket():
    try:
        sk = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sk.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sk.bind(SER_ADDR)
        sk.listen(10)
    except socket.error as msg:
        print(msg)
        sys.exit(1)

    print('server waiting...')
    while True:
        conn,addr = sk.accept()
        t = threading.Thread(target = handle_data, args = (conn,addr))
        t.start()

def handle_data(conn, addr):
    print('Accept new connection from {0}'.format(addr))
    conn.send(('Hi, Welcome to the server!').encode())

    while True:
        client_data = conn.recv(BUFFSIZE)
        print('{0} client send data is {1}'.format(addr, client_data.decode()))
        time.sleep(1)
        if client_data.decode() == 'exit' or not client_data:
            print('{0} connection close'.format(addr))
            conn.send( ('Connection closed!').encode() )
            break

        massage = input("Server : ").strip()
        conn.send( ("Server : "+massage).encode() )

    conn.close()

if __name__ == "__main__":
    server_socket()

#socket server
