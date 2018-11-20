import socket
import sys
from contextlib import closing

def main(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    #  with closing(sock):
    sock.bind((host, int(port)))
    while True:
        hoge = sock.recv(1024).decode('utf-8')
        print(hoge)
        #  sys.stdout.write(hoge)
    #  return

if __name__ == '__main__':
    host = sys.argv[1]
    if sys.argv[1] == 'broadcast':
        host = ""
    #  print("host: " + host)
    #  print("port: " + sys.argv[2])
    main(host, sys.argv[2])

