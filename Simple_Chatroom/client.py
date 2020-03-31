import sys
from socket import *
from select import select
from multiprocessing import Process
from settings import *

def connect(sock_client, pipe_server, name):
    # IO多路复用：循环监听套接字
    rlist = [sock_client, pipe_server]
    wlist = []
    xlist = []

    while True:
        rs, ws, xs = select(rlist, wlist, xlist)

        for r in rs:
            if r is sock_client:
                # 接受服务端的信息
                data = sock_client.recv(BUFFERSIZE).decode()
                print(data, end="")
            elif r is pipe_server:
                # 接受键盘输入并发送给服务端
                conn, addr = pipe_server.accept()
                data = conn.recv(BUFFERSIZE)
                data = bytes(name + "：", "UTF-8") + data
                sock_client.send(data)
                conn.close()

def get_name():
    return input("User name: ")

if __name__ == '__main__':
    # 使用get_name函数获得用户名 
    name = get_name()

    # 创建两个套接字
    # 套接字sock_client是一个TCP客户端，负责服务端与客户端的交流
    # 套接字pipe_server也是一个TCP服务端，不过起到管道的作用，负责接收键盘输入
    sock_client = client(SOCK_ADDR)
    sock_client.send(bytes(name + "加入了聊天室。\n", "UTF-8"))
    pipe_server = server(CLI_PIPE_ADDR)

    # 开始一个子进程，执行connect函数
    p = Process(target=connect, args=(sock_client, pipe_server, name))
    p.daemon = True
    p.start()

    # 循环接收键盘输入
    while True:
        try:
            # 从标准输入流（键盘）读取一行
            data = sys.stdin.readline()
        except KeyboardInterrupt:
            # 如果遇到退出/中止信号，发送退出信息，关闭套接字，结束子进程，退出程序
            sock_client.send(bytes(name + "退出了聊天室。\n", "UTF-8"))
            sock_client.close()
            pipe_server.close()
            p.terminate()
            break

        if not data:
            # 如果从键盘获取数据为空，继续循环
            continue
        else:
            # 获得键盘数据，创建客户端套接字pipe_client，将键盘输入传输给pipe_server
            pipe_client = client(CLI_PIPE_ADDR)
            pipe_client.send(bytes(data, "UTF-8"))
            pipe_client.close()