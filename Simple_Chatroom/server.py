import sys
import shelve
from socket import *
from select import select
from multiprocessing import Process
from settings import *

def listen(sock_server, pipe_server):
    # IO多路复用：循环监听套接字
    rlist = [sock_server, pipe_server]
    wlist = []
    xlist = []

    print("等待连接...")
    while True:
        rs, ws, xs = select(rlist, wlist, xlist)

        for r in rs:
            if r is sock_server:
                # 接受客户端连接
                conn, addr = sock_server.accept()
                rlist.append(conn)
            elif r is pipe_server:
                # 接收键盘输入并发送到所有客户端去
                conn, addr = pipe_server.accept()
                data = conn.recv(BUFFERSIZE)
                data = bytes("管理员：", "UTF-8") + data
                for c in rlist[2:]:
                    c.send(data)
                conn.close()
            else:
                # 接收客户端信息
                # 将客户端信息发送到所有的客户端中去
                try:
                    data = r.recv(BUFFERSIZE)
                except:
                    r.close()
                    rlist.remove(r)
                else:
                    print(data.decode(), end="")
                    for c in rlist[2:]:
                        c.send(data)

def clear_all():
    f = shelve.open("ports")
    f['ports'].clear()
    f.close()

if __name__ == '__main__':
    # 首先将ports内容都删除
    clear_all()

    # 创建两个套接字
    # 套接字sock_server是一个TCP服务端，负责服务端与客户端的交流
    # 套接字pipe_server也是一个TCP服务端，不过起到管道的作用，负责接收键盘输入
    sock_server = server(SOCK_ADDR)
    pipe_server = server(SER_PIPE_ADDR)

    # 开始一个子进程，执行listen函数
    p = Process(target=listen, args=(sock_server, pipe_server))
    p.daemon = True
    p.start()
    
    # 循环接收键盘输入
    while True:
        try:
            # 从标准输入流（键盘）读取一行
            data = sys.stdin.readline()  
        except KeyboardInterrupt:
            # 如果遇到退出/中止信号，关闭套接字，结束子进程，退出程序
            sock_server.close()
            pipe_server.close()
            p.terminate()
            clear_all()
            break

        if not data:
            # 如果从键盘获取数据为空，继续循环
            continue
        else:
            # 获得键盘数据，创建客户端套接字pipe_client，将键盘输入传输给pipe_server
            pipe_client = client(SER_PIPE_ADDR)
            pipe_client.send(bytes(data, "UTF-8"))
            pipe_client.close()