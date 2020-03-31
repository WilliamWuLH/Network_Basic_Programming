#coding:utf-8

import socket
import sys
import os

ip_port = ('127.0.0.1',9999)
sk = socket.socket()
sk.connect(ip_port)

container = {'key':'','data':''}
while True:
    # 客户端输入要上传文件的路径
    path = input('path:')
    # 根据路径获取文件名
    file_name = os.path.basename(path)
    #print(file_name)
    # 获取文件大小
    file_size=os.stat(path).st_size
    # 发送文件名 和 文件大小
    Informf=(file_name+'|'+str(file_size))
    sk.send(Informf.encode())
    # 为了防止粘包，将文件名和大小发送过去之后，等待服务端收到，直到从服务端接受一个信号（说明服务端已经收到）
    sk.recv(1024)
    send_size = 0
    f= open(path,'rb')
    Flag = True
    while Flag:
        if send_size + 1024 >file_size:
            data = f.read(file_size-send_size)
            Flag = False
        else:
            data = f.read(1024)
            send_size+=1024
        sk.send(data)
    f.close()
    
sk.close()

#FTP上传文件（客户端）
