import socket
import sys
from contextlib import closing

def main(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    with closing(sock):
        while True:
            msg = input().encode('utf-8')
            sock.sendto(msg, (host, int(port)))
            print(msg.decode('utf-8'))
    return

if __name__ == '__main__':
    #  print("host: " + sys.argv[1])
    #  print("port: " + sys.argv[2])
    main(sys.argv[1], sys.argv[2])

