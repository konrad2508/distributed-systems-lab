import socket
import struct
import datetime
import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise ValueError("Wrong number of the log file!")

    fileN = sys.argv[1]

    # create socket
    clientsSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
    clientsSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    clientsSocket.bind(('', 5000))

    # prepare for multicast communication
    mreq = struct.pack("=4sl", socket.inet_aton("224.0.0.3"), socket.INADDR_ANY)
    clientsSocket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    # start logging loop
    while True:
        # read from socket
        data, address = clientsSocket.recvfrom(1024)
        data = str(data)[2:(str(data).index('\\'))]
        string = str(datetime.datetime.now()) + ": " + data

        # write to the file
        f = open("logfile" + str(fileN) + ".txt", "a+")
        f.write(string + "\n")
        f.close()
