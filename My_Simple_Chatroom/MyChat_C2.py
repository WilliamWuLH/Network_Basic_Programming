import socket
import sys
import os

ip_port = ('127.0.0.1',6666)

def FTP_send(sk):
    path = input('Path:')
    file_name = os.path.basename(path)
    file_size=os.stat(path).st_size
    Informf=(file_name+'|'+str(file_size))
    sk.send(Informf.encode())
    receive = sk.recv(1024)
    if receive == 'n':
        print("OVER")
        return
    send_size = 0
    with open(path,'rb') as f:
        Flag = True
        while Flag:
            if send_size + 1024 >file_size:
                data = f.read(file_size-send_size)
                Flag = False
            else:
                data = f.read(1024)
                send_size+=1024
            sk.send(data)
        print('send success')
        f.close()

def FTP_receive(sk, pre_data):
    base_path = 'F:\\3大三下\\计算机网络\\实验课\\实验2\\'
    file_name,file_size = pre_data.split('|')
    print("Have a file  -->  name : ", file_name, " size : ", file_size)
    ans = input("Do you receive ? (y/n)   ")
    sk.sendall(ans.encode())
    if ans == 'n':
        print("OVER")
        return           
    recv_size = 0
    file_dir = os.path.join(base_path,file_name)
    with open(file_dir,'wb') as f:
        Flag = True
        while Flag:
            if int(file_size)>recv_size:
                data = sk.recv(1024) 
                recv_size+=len(data)
                f.write(data)
            else:
                recv_size = 0
                Flag = False    
        print('receive successed !')
        f.close()

while True:
    sk = socket.socket()
    sk.connect(ip_port)
    model = input("MODEL:    1:chat    2:send file    3:exit\nYour choice : ")
    if model == '1':
        massage = input("Client_2 : ").strip()
        sk.sendall(("Client_2 : " + massage).encode())
    elif model == '2':
        FTP_send(sk)
    elif model == '3':
        break
    else:
        print('error !')
        break
    print("client_2 waiting...")
    
    server_reply = sk.recv(1024).decode()
    if '|' in server_reply:
        FTP_receive(sk, server_reply)
    else:
        print(server_reply)

    sk.close()

#F:\\3大三下\\计算机网络\\实验课\\实验2\\test.txt