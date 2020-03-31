import socket
import sys
import os

ip_port = ('127.0.0.1',6666)

sk = socket.socket()
sk.bind(ip_port)
sk.listen(5)

def FTP_send(conn):
    path = input('Path:')
    file_name = os.path.basename(path)
    file_size=os.stat(path).st_size
    Informf=(file_name+'|'+str(file_size))
    conn.send(Informf.encode())
    receive = conn.recv(1024).decode()
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
            conn.send(data)
        print('send successed !')
        f.close()

def FTP_receive(conn, pre_data):
    base_path = 'C:\\Users\\William Wu\\Desktop'
    file_name,file_size = pre_data.split('|')
    print("Have a file  -->  name : ", file_name, " size : ", file_size)
    ans = input("Do you receive ? (y/n)   ")
    conn.sendall(ans.encode())
    if ans == 'n':
        print("OVER")
        return          
    recv_size = 0
    file_dir = os.path.join(base_path,file_name)
    with open(file_dir,'wb') as f:
        Flag = True
        while Flag:
            if int(file_size)>recv_size:
                data = conn.recv(1024) 
                recv_size+=len(data)
                f.write(data)
            else:
                recv_size = 0
                Flag = False    
        print('receive successed !')
        f.close()

while True:
    print('client_1 waiting...')
    conn,addr = sk.accept()

    client_data = conn.recv(1024).decode()
    if '|' in client_data:
        FTP_receive(conn, client_data)
    else:
        print(client_data)

    model = input("MODEL:    1:chat    2:send file    3:exit\nYour choice : ")
    if model == '1':
        massage = input("Client_1 : ").strip()
        conn.sendall(("Client_1 : "+massage).encode())
    elif model == '2':
        FTP_send(conn)
    elif model == '3':
        conn.close()
        break
    else:
        print('error !')
        conn.close()
        break

#C:\\Users\\William Wu\\Desktop\\WLH.txt