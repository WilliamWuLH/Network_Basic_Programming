import socket
ip_port = ('127.0.0.1',5000)

sk = socket.socket(socket.AF_INET,socket.SOCK_DGRAM,0)
while True:
    data = input('数据：').strip()
    if data == 'exit':
        break
    sk.sendto(data.encode(),ip_port)

sk.close()

#UDP Demo
