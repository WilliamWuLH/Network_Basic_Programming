import socket
import select
import time
import sys

IP = '127.0.0.1'
PORT = 6666
SER_ADDR = IP,PORT
BUFFSIZE = 1024

def server_select():
    try:
        sks = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sks.bind(SER_ADDR)
        sks.listen(10)
    except socket.error as msg:
        print(msg)
        sys.exit(1)

    rlist = [sks,]
    wlist = []
    xlist = []
    while True:
        print("selecting...")
        rs,ws,xs = select.select(rlist, wlist, xlist)

        for sk in rs:
            if sk is sks:
                conn, addr = sks.accept()
                print('Accept new connection from {0}'.format(addr))
                conn.send( ('Connect success!').encode() )
                rlist.append(conn)
            else:
                try:
                    data = sk.recv(BUFFSIZE)
                except:
                    sk.close()
                    rlist.remove(sk)
                else:
                    print("{0} : {1}".format(sk.getsockname, data.decode()))
                    for c in rlist[1:]:
                        c.send(data)
            
        for sk in ws:
            pass

        for sk in xs:
            rlist.remove(sk)

if __name__ == "__main__":
    server_select()
