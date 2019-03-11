import socket
import struct
import datetime

clientsSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
clientsSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
clientsSocket.bind(('', 5000))

mreq = struct.pack("=4sl", socket.inet_aton("224.0.0.3"), socket.INADDR_ANY)
clientsSocket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

while True:
    data, address = clientsSocket.recvfrom(1024)
    data = str(data)[2:(str(data).index('\\'))]
    print(str(datetime.datetime.now()) + ": " + data)
